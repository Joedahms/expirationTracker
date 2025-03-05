#ifndef HARDWARE_H
#define HARDWARE_H

#include <zmqpp/zmqpp.hpp>

#include "../../logger.h"
#include "../../pipes.h"

class Hardware {
public:
  Hardware(zmqpp::context& context, const struct ExternalEndpoints& externalEndpoints);

  bool checkStartSignal();
  bool startScan();
  void sendDataToVision();

private:
  ExternalEndpoints externalEndpoints;

  Logger logger;

  zmqpp::socket requestVisionSocket;
  zmqpp::socket requestDisplaySocket;
  zmqpp::socket replySocket;

  const std::string IMAGE_DIRECTORY = "/home/pi/Documents/raspi-yolo/images/temp/";
  // Code for when testing on different pi
  //  const std::string IMAGE_DIRECTORY =
  //  "/home/geromy/Desktop/Project/raspi-yolo/images/temp/";

  float itemWeight = 0;

  void rotateAndCapture();
  bool capturePhoto(int angle);
  bool checkWeight();
};

#endif
