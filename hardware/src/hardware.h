#ifndef HARDWARE_H
#define HARDWARE_H

#include <zmqpp/zmqpp.hpp>

#include "../../endpoints.h"
#include "../../logger.h"

#define MOTOR_IN1 23 // GPIO Pin for L298N IN1
#define MOTOR_IN2 24 // GPIO Pin for L298N IN2
#define MOTOR_ENA 18 // GPIO Pin forL298N enable (PWM Speed Control)

// TODO Figure out a better way to represent the photo path
//
// There is the constant below IMAGE_DIRECTORY but ../images/temp is also
// used in sendDataToVision. Need a way to better define the photo path.
// Possibly even in main.cpp so that it can be passed to vision as well.

class Hardware {
public:
  Hardware(zmqpp::context& context);

  void initDC();
  bool checkStartSignal(int timeoutMs);
  void sendStartToVision();
  bool startScan();

private:
  Logger logger;

  zmqpp::socket requestVisionSocket;
  zmqpp::socket requestDisplaySocket;
  zmqpp::socket replySocket;

  std::filesystem::path imageDirectory;

  float itemWeight = 0;

  bool checkWeight();
  void rotateAndCapture();
  bool takePhotos(int angle);
  void rotateMotor(bool clockwise);
  bool capturePhoto(int angle);
};

#endif
