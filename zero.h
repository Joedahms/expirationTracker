#ifndef ZERO_H
#define ZERO_H

#include <string>
#include <zmqpp/zmqpp.hpp>

#include "logger.h"

std::string sendMessage(zmqpp::socket& socket,
                        const std::string& message,
                        Logger& logger);
std::string receiveMessage(zmqpp::socket& socket,
                           const std::string& response,
                           const int timeoutMs,
                           Logger& logger);

#endif
