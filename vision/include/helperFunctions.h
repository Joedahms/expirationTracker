#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

#include <algorithm>
#include <arpa/inet.h>
#include <filesystem>
#include <glog/logging.h>
#include <iostream>
#include <string>
#include <vector>

#include "../../endpoints.h"

#define BROADCAST_IP        "255.255.255.255"
#define ZEROMQPORT          5555
#define ZEROMQHEARTBEATPORT 5556
#define DISCOVERY_PORT      5005

bool isValidDirectory(const std::filesystem::path&);
bool hasFiles(const std::filesystem::path&);
std::string toLowerCase(std::string);
std::vector<std::string> splitWords(const std::string&);
std::string cleanText(const std::string&);
std::string removePrefix(const std::string&, const std::string&);
std::string joinVector(const std::vector<std::string>&, const std::string&);
std::string discoverServerViaUDP();

#endif
