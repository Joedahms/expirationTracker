#ifndef HARDWARE_H
#define HARDWARE_H

#include <zmqpp/zmqpp.hpp>

#include "../../endpoints.h"
#include "../../food_item.h"
#include "../../logger.h"
#include "hardware_flags.h"

class Hardware {
public:
  Hardware(zmqpp::context& context, const HardwareFlags& hardwareFlags);

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

  const int MOTOR_IN1 = 23;
  const int MOTOR_IN2 = 24;

  const bool usingMotor;
  const bool usingCamera;

  void rotateAndCapture();
  bool takePhotos(int angle);
  void rotateMotor(bool clockwise);
  bool capturePhoto(int angle);
};

#endif
