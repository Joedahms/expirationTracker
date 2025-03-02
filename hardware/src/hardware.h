#ifndef HARDWARE_H
#define HARDWARE_H

#include <zmqpp/zmqpp.hpp>

#include "../../pipes.h"

class Hardware {
public:
  Hardware(zmqpp::context& context, const struct ExternalEndpoints& externalEndpoints);

  bool checkStartSignal();
  void startScan();

private:
  ExternalEndpoints externalEndpoints;

  zmqpp::socket requestVisionSocket;
  zmqpp::socket requestDisplaySocket;
  zmqpp::socket replySocket;
};

#endif
