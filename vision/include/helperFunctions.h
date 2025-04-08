#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

#include "../../logger.h"
#include <algorithm>
#include <arpa/inet.h>
#include <filesystem>
#include <fstream>
#include <glog/logging.h>
#include <ifaddrs.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "../../endpoints.h"
#include "config.h"

#define BROADCAST_IP   "255.255.255.255"
#define DISCOVERY_PORT 5005

bool isValidDirectory(const std::filesystem::path&);
bool hasFiles(const std::filesystem::path&);
std::string toLowerCase(std::string);
std::vector<std::string> splitWords(const std::string&);
std::string cleanText(const std::string&);
std::string removePrefix(const std::string&, const std::string&);
std::string joinVector(const std::vector<std::string>&, const std::string&);
std::string discoverServerViaUDP(const Logger&);
std::string getEthernetIP(const std::string&, const Logger&);
Config loadConfig(const std::filesystem::path&);

#endif
