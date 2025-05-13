#ifndef MESSENGER_H
#define MESSENGER_H

#include <string>
#include <zmqpp/zmqpp.hpp>

#include "logger.h"

class Messenger {
public:
  std::string sendMessage(zmqpp::socket& socket,
                          const std::string& message,
                          Logger& logger);
  std::string receiveMessage(zmqpp::socket& socket,
                             const std::string& response,
                             const int timeoutMs,
                             Logger& logger);
};

#endif
