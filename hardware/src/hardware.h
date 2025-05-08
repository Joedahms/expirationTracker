#ifndef HARDWARE_H
#define HARDWARE_H

#include <zmqpp/zmqpp.hpp>

#include "../../endpoints.h"
#include "../../food_item.h"
#include "../../logger.h"
#include "hardware_flags.h"
#include "network.h"

class Hardware {
public:
  Hardware(zmqpp::context& context, const HardwareFlags& hardwareFlags);

  void start();

private:
  void initDC();
  bool checkStartSignal(int timeoutMs);
  bool startScan();

  const int DISCOVERY_PORT      = 5005;
  const std::string SERVER_PORT = "5555";

  Logger logger;
  Network network;

  zmqpp::socket requestServerSocket;
  zmqpp::socket requestDisplaySocket;
  zmqpp::socket replySocket;

  std::filesystem::path imageDirectory;

  const int MOTOR_IN1 = 23;
  const int MOTOR_IN2 = 24;

  const bool usingMotor;
  const bool usingCamera;

  int angle = 0;

  void rotateAndCapture();
  void takePhotos();
  void rotateMotor(bool clockwise);
};

#endif
