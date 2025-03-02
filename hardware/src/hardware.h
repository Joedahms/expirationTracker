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

  const std::string IMAGE_DIRECTORY = "/home/geromy/Desktop/Project/raspi-yolo/images/";

  void sendDataToVision(float weight);
  void rotateAndCapture(float weight);
  bool capturePhoto(int angle);
};

#endif
