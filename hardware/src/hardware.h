#ifndef HARDWARE_H
#define HARDWARE_H

#include <zmqpp/zmqpp.hpp>

#include "../../pipes.h"

class Hardware {
public:
  Hardware(zmqpp::context& context, const struct ExternalEndpoints& externalEndpoints);

  bool checkStartSignal();
  bool startScan();

private:
  ExternalEndpoints externalEndpoints;

  zmqpp::socket requestVisionSocket;
  zmqpp::socket requestDisplaySocket;
  zmqpp::socket replySocket;

  const std::string IMAGE_DIRECTORY = "/home/pi/Documents/raspi-yolo/images/";

  float itemWeight = 0;

  void sendDataToVision();
  void rotateAndCapture();
  bool capturePhoto(int angle);
  bool checkWeight();
};

#endif
