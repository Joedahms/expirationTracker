#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <string>
#include <unistd.h>
// #include <wiringPi.h> // wiringPi.h can be found after install

#include "../../../WiringPi/wiringPi/wiringPi.h"

#define MOTOR_IN1 23 // GPIO Pin for L298N IN1
#define MOTOR_IN2 24 // GPIO Pin for L298N IN2

/**
 * Initializes GPIO, hardware and sensors.
 * If compiled with gcc needs link
 *      Ex: gcc -o myapp myapp.c -l wiringPi
 * @return None
 */
void setupMotor() {
  LOG(INFO) << "Motor System Initialization...";

  // uses BCM numbering of the GPIOs and directly accesses the GPIO registers.
  wiringPiSetupGpio();

  // pin mode ..(INPUT, OUTPUT, PWM_OUTPUT, GPIO_CLOCK)
  // Setup DC Motor Driver Pins
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);

  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);

  LOG(INFO) << "Motor System Initialized.";
}

/**
 * Controls platform rotation with L298N motor driver.
 * Rotates for a fixed duration.
 * According to current calculations in hardware-chat:
 * 10RPM = 4RPM -> ~ 1.875 sec for 45 degrees || 15 sec for full rotation.
 * HIGH,LOW == forward LOW,HIGH == backwards
 * @param clockwise Boolean; true for clockwise, false for counterclockwise
 *
 * @return None
 */
// likely needs to be updated after testing to confirm direction
void rotateMotor(bool clockwise) {
  if (clockwise) {
    LOG(INFO) << "Rotating clockwise...";
    digitalWrite(MOTOR_IN1, HIGH);
    digitalWrite(MOTOR_IN2, LOW);
    usleep(1875000); // Rotate duration
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, LOW); // HIGH,HIGH || LOW,LOW == off
  }
  else {
    LOG(INFO) << "Rotating counterclockwise...";
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, HIGH);
    usleep(1875000);
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, LOW);
  }
  LOG(INFO) << "Rotation complete.";
}