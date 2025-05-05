#ifndef NETWORK_H
#define NETWORK_H

#include <zmqpp/zmqpp.hpp>

#include "../../logger.h"

class Network {
public:
  Network(const std::string& configFilename);
  void connectToServer(zmqpp::socket& socket, Logger& logger);

private:
  std::string getServerIp();

  int serverPort    = -1;
  int discoveryPort = -1;
};

#endif
