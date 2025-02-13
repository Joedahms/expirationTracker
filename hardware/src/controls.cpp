// #include "HX711.h"
#include "../../food_item.h"
#include "hardware_entry.h"
#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <wiringPi.h> // Raspberry Pi 5 library for GPIO in C++

#define MOTOR_IN1      23 // GPIO Pin for L298N IN1
#define MOTOR_IN2      24 // GPIO Pin for L298N IN2
#define MOTOR_ENA      25 // GPIO Pin forL298N enable (PWM Speed Control)
#define LOAD_CELL_DOUT 6  // HX711 Data pin
#define LOAD_CELL_SCK  5  // HX711 Clock pin
/**
 * Sample command. rpicam-apps allows bulding customized behavior
 * run libcamera-hello --list-cameras to get camera names
 */
#define IMAGE_DIR   "../../images/image_"
#define CAMERA1_CMD "rpicam-still --camera 0 -o "
#define CAMERA2_CMD "rpicam-still --camera 1 -o "

extern HardwarePipes pipes;

HX711 scale;

/**
 * Initializes GPIO, hardware and sensors.
 * If compiled with gcc needs link
 *      Ex: gcc -o myapp myapp.c -l wiringPi
 * @return None
 */
void setup() {
  LOG(INFO) << "Control system initialization...";

  // uses BCM numbering of the GPIOs and directly accesses the GPIO registers.
  wiringPiSetupGpio();

  // pin mode ..(INPUT, OUTPUT, PWM_OUTPUT, GPIO_CLOCK)
  // Setup DC Motor Driver Pins
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(MOTOR_ENA, OUTPUT); // PWM_OUTPUT?

  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);
  analogWrite(MOTOR_ENA, 128); // Set inital speed to 50% (0-255 range)

  // Setup HX711 Weight Sensor
  scale.begin(LOAD_CELL_DOUT, LOAD_CELL_SCK);
  scale.set_scale(0.0); // Adjustable calibration factor (varies +-250k)
  scale.tare();         // Zero the scale

  LOG(INFO) << "Control System Initialized.";
}

/**
 * Controls platform rotation with L298N motor driver.
 * Rotates for a fixed duration.
 * According to current calculations in hardware-chat:
 * 10RPM = 4RPM -> ~ 2 sec for 45 degrees || 15 sec for full rotation.
 *
 * @return None
 */
void rotateMotor() {
  digitalWrite(MOTOR_IN1, HIGH); // HIGH,LOW == forward LOW,HIGH == backwards
  digitalWrite(MOTOR_IN2, LOW);
  analogWrite(MOTOR_ENA, 255); // Adjust speed
  usleep(2000000);             // Rotate for 2 sec (roughly 45 degrees)
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW); // HIGH,HIGH || LOW,LOW == off
}

/**
 * Takes photos from both camera modules simultaneously using separate processes.
 * Photos should go to "../../images"
 * Image format "image_x_T" || "image_x_S", where x == angle.
 *
 * @param angle - The angle at which the photos are taken. (0-8, where 8==0)
 * @return None
 */
void takePhotos(int angle) {
  pid_t top_cam = fork();
  if (top_cam == -1) {
    LOG(FATAL) << "Error starting top camera process.";
  }
  else if (top_cam == 0) {
    std::string top_photo =
        CAMERA1_CMD + IMAGE_DIR + std::to_string(angle) + "_T.jpg --nopreview";
    if (system(top_photo.c_str()) == -1) {
      LOG(FATAL) << "Failed to capture image from top camera.";
    }
    exit(0);
  }

  pid_t side_cam = fork();
  if (side_cam == -1) {
    LOG(FATAL) << "Error starting side camera process.";
  }
  else if (side_cam == 0) {
    std::string side_photo =
        CAMERA1_CMD + IMAGE_DIR + std::to_string(angle) + "_S.jpg --nopreview";
    if (system(side_photo.c_str()) == -1) {
      LOG(FATAL) << "Failed to capture image from side camera.";
    }
    exit(0);
  }

  waitpid(top_cam, NULL, 0);
  waitpid(side_cam, NULL, 0);

  LOG(INFO) << "Photos successful at position " << angle;
}

/**
 * Takes a photo using the Raspberry Pi camera module.
 *
 * @param filename The name of the file to save the photo to.
 * @return None
 */
void takePic(char* filename) {
  static pid_t pid = 0;

  if ((pid = fork()) == 0) {
    execl("/usr/bin/raspistill", "usr/bin/raspistill", "-n", "-vf", "-o", filename, NULL);
  }
}

/**
 * Sends the photo directory and weight data to the AI Vision system.
 *
 * @param IMAGE_DIR Directory containing captured images.
 * @param weight The weight of the object on the platform.
 * @return None
 */
void sendDataToVision(const std::string IMAGE_DIR, float weight) {
  LOG(INFO) << "Sending Images from Hardware to Vision";

  struct FoodItem item;
  item.photoPath      = IMAGE_DIR;
  item.name           = "";
  item.scanDate       = std::chrono::floor<std::chrono::days>(now);
  item.expirationDate = "year_month_day";
  item.catagory       = "";
  item.weight         = weight;
  item.quantity       = 1;
  sendFoodItem(item, pipes.toVision[WRITE]);

  LOG(INFO) << "Done Sending Images from Hardware to Vision";
}

/**
 * Rotates platform in 45-degree increments, captures images, and sends the
 * data to the AI Vision system until a full 360-degree rotation is complete.
 * The process can be stopped early if AI Vision sends a "STOP" signal.ADJ_FREQUENCY
 *
 * @return None
 */
void rotateAndCapture() {
  float weight = scale.get_units(10);
  LOG(INFO) << "Reading Weight: " << weight << " g";

  for (int angle = 0; angle < 8; angle++) {
    LOG(INFO) << "Capturing at position " << angle;
    /**
     * char filename[] = "IMAGE_DIR + std::to_string(angle) + "_S.jpg --nopreview";
     * takePic(filename);
     * cout << "Captured image at position " << angle << endl;
     */
    takePhotos(angle);
    sendDataToVision(IMAGE_DIR, weight);

    LOG(INFO) << "Rotating platform...";
    rotateMotor();
    usleep(2);

    char stop_signal[10];
    if (read(hardwarePipes.fromVision[READ], stop_signal, sizeof(stop_signal)) > 0) {
      LOG(INFO) << "AI Vision identified item. Stopping process.";
      break;
    }
  }
}
