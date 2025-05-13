#include <arpa/inet.h>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "../../messenger.h"
#include "network.h"

Network::Network(const std::string& configFilename) {
  std::filesystem::path currentPath = std::filesystem::current_path();
  std::filesystem::path filePath    = currentPath / configFilename;
  std::ifstream file(filePath);

  if (!file.is_open()) {
    std::cerr << "Error: Could not open the file." << std::endl;
  }

  nlohmann::json config = nlohmann::json::parse(file);

  this->serverPort    = config["network"]["serverPort"];
  this->discoveryPort = config["network"]["discoveryPort"];
}

void Network::connectToServer(zmqpp::socket& socket, Logger& logger) {
  std::string serverIp = getServerIp();
  std::string serverAddress =
      "tcp://" + serverIp + ":" + std::to_string(this->serverPort);
  socket.connect(serverAddress);
  std::string log = "Connected to server on " + serverAddress;
  logger.log(log);
  std::cout << log << std::endl;

  Messenger messenger;
  messenger.sendMessage(socket, "connected", logger);
}

std::string Network::getServerIp() {
  int sockfd;
  struct sockaddr_in serverAddr;
  socklen_t addrLen = sizeof(serverAddr);
  char buffer[1024] = {0};

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    std::cerr << "UDP socket creation failed";
    return "";
  }

  int broadcastEnable = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));

  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family      = AF_INET;
  serverAddr.sin_addr.s_addr = inet_addr("255.255.255.255");
  serverAddr.sin_port        = htons(this->discoveryPort);

  std::string message = "DISCOVER_SERVER";
  sendto(sockfd, message.c_str(), message.size(), 0, (struct sockaddr*)&serverAddr,
         addrLen);

  std::cout << "Discovery request sent, waiting for response..." << std::endl;
  recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&serverAddr, &addrLen);
  close(sockfd);

  return std::string(buffer);
}
