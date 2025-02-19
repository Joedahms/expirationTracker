#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * Sample command. rpicam-apps allows bulding customized behavior
 * run libcamera-hello --list-cameras to get camera names
 */
#define IMAGE_DIR "/home/pi/Desktop/Project/raspi-yolo/images/";

/**
 * Takes photos from both camera modules simultaneously using separate processes.
 * Photos should go to "../../images"
 * Image format "image_x_T" || "image_x_S", where x == angle.
 *
 * @param angle - The angle at which the photos are taken. (0-8, where 8==0)
 * @return None
 */
void takePhotos(int angle) {
  pid_t topCam = fork();
  if (topCam == -1) {
    LOG(FATAL) << "Error starting top camera.";
  }
  else if (topCam == 0) {
    std::string filepath = std::string(IMAGE_DIR) + std::to_string(angle) + "_T.jpg";
    execl("/usr/bin/rpicam-still", "rpicam-still",
          // Full resolution
          "--width", "4056", "--height", "3040",
          "--nopreview",                // Disable preview
          "--autofocus",                // Auto-focus enabled
          "--exposure", "auto",         // Auto exposure
          "--output", filepath.c_str(), // Save location
          "--timeout", "1000",          // 1 second timeout
          // "--roi 0.25,0.25,0.5,0.5",    // Zoom into the center 50%
          (char*)NULL);

    std::cerr << "Error: Failed to execute rpicam-still" << std::endl;
    exit(1);
    if (system(filepath.c_str()) == -1) {
      LOG(FATAL) << "Failed to capture image from top camera.";
    }
    exit(0);
  }

  pid_t sideCam = fork();
  if (sideCam == -1) {
    LOG(FATAL) << "Error starting side camera process.";
  }
  else if (sideCam == 0) {
    std::string filepath = std::string(IMAGE_DIR) + std::to_string(angle) + "_T.jpg";
    execl("/usr/bin/rpicam-still", "rpicam-still",
          // Full resolution
          "--width", "4056", "--height", "3040",
          "--nopreview",                // Disable preview
          "--autofocus",                // Auto-focus enabled
          "--exposure", "auto",         // Auto exposure
          "--output", filepath.c_str(), // Save location
          "--timeout", "1000",          // 1 second timeout
          // "--roi 0.25,0.25,0.5,0.5",    // Zoom into the center 50%
          (char*)NULL);

    std::cerr << "Error: Failed to execute rpicam-still" << std::endl;
    exit(1);
    if (system(filepath.c_str()) == -1) {
      LOG(FATAL) << "Failed to capture image from top camera.";
    }
    exit(0);
  }

  waitpid(topCam, NULL, 0);
  waitpid(sideCam, NULL, 0);
  LOG(INFO) << "Photos successful at position " << angle;
}

/**
 * Takes a photo using the Raspberry Pi camera module.
 * https://github.com/jeffmachyo/Raspberry-Pi-Camera-C-and-CPP.git
 *
 * @param filename The name of the file to save the photo to.
 * @return None
 */
static pid_t pid = 0;

void takePic(char* filename) {
  if ((pid = fork()) == 0) {
    execl("/usr/bin/raspistill", "/usr/bin/raspistill", "-n", "-o", filename, NULL);
  }
}