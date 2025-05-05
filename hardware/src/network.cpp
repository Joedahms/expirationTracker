#include <arpa/inet.h>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "network.h"

void connectToServer() {
  std::filesystem::path currentPath = std::filesystem::current_path();
  std::filesystem::path filePath    = currentPath / "../network_config.json";

  std::ifstream file(filePath);

  // Check if file opened successfully
  if (!file.is_open()) {
    std::cerr << "Error: Could not open the file." << std::endl;
  }

  // Parse the JSON from the file stream
  try {
    nlohmann::json data = nlohmann::json::parse(file);

    // Example: Iterate through an array
    if (data.contains("network") && data["network"].is_array()) {
      for (const auto& item : data["network"]) {
        std::cout << "Item: " << item << std::endl;
      }
    }

  } catch (const nlohmann::json::parse_error& e) {
    std::cerr << "JSON parsing error: " << e.what() << std::endl;
  }

  /*
  std::string serverIp      = getServerIp();
  std::string serverAddress = "tcp://" + serverIp + ":" + this->SERVER_PORT;
  this->requestServerSocket.connect(serverAddress);
  std::string log = "Connected to server on " + serverAddress;
  this->logger.log(log);
  std::cout << log << std::endl;
  sendMessage(this->requestServerSocket, "connected", this->logger);
  */
}

std::string getServerIp() {
  int sockfd;
  struct sockaddr_in serverAddr;
  socklen_t addrLen = sizeof(serverAddr);
  char buffer[1024] = {0};

  // Create UDP socket
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    std::cerr << "UDP socket creation failed";
    return "";
  }

  // Enable broadcast mode
  int broadcastEnable = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));

  // Set destination address
  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family      = AF_INET;
  serverAddr.sin_addr.s_addr = inet_addr("255.255.255.255");
  //  serverAddr.sin_port        = htons(this->DISCOVERY_PORT);
  serverAddr.sin_port = htons(5555);

  // Send discovery request
  std::string message = "DISCOVER_SERVER";
  sendto(sockfd, message.c_str(), message.size(), 0, (struct sockaddr*)&serverAddr,
         addrLen);

  std::cout << "Discovery request sent, waiting for response..." << std::endl;

  // Wait for response
  recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&serverAddr, &addrLen);
  close(sockfd);

  return std::string(buffer);
}
