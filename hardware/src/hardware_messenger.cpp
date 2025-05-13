#include <arpa/inet.h>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "../../endpoints.h"
#include "hardware_messenger.h"

HardwareMessenger::HardwareMessenger(zmqpp::context& context,
                                     const std::string& configFilename,
                                     Logger& logger)
    : requestServerSocket(context, zmqpp::socket_type::request),
      requestDisplaySocket(context, zmqpp::socket_type::request),
      replySocket(context, zmqpp::socket_type::reply) {
  std::filesystem::path currentPath = std::filesystem::current_path();
  std::filesystem::path filePath    = currentPath / configFilename;
  std::ifstream file(filePath);

  if (!file.is_open()) {
    std::cerr << "Error: Could not open the file." << std::endl;
  }

  nlohmann::json config = nlohmann::json::parse(file);

  this->serverPort    = config["network"]["serverPort"];
  this->discoveryPort = config["network"]["discoveryPort"];

  try {
    connectToServer(this->requestServerSocket, logger);
    this->requestDisplaySocket.connect(ExternalEndpoints::displayEndpoint);
    this->replySocket.bind(ExternalEndpoints::hardwareEndpoint);
  } catch (const zmqpp::exception& e) {
    std::cerr << e.what();
    exit(1);
  }
}

/**
 * Checks for a start signal from the display. Return if have not received a message by
 * the timeout.
 *
 * @param None
 * @return bool - True if start signal received, false otherwise
 */
bool HardwareMessenger::checkStartSignal(const int timeoutMs, Logger& logger) {
  logger.log("Checking for start signal from display");

  std::string incomingMessage;
  incomingMessage =
      receiveMessage(this->replySocket, Messages::AFFIRMATIVE, timeoutMs, logger);

  if (incomingMessage == Messages::START_SCAN) {
    logger.log("Received start signal from display");
    return true;
  }
  else {
    logger.log("Did not receive start signal from display");
    return false;
  }
}

bool HardwareMessenger::checkStopSignal(const int timeoutMs, Logger& logger) {
  logger.log("Checking for stop signal");

  std::string incomingMessage;
  incomingMessage =
      receiveMessage(this->replySocket, Messages::AFFIRMATIVE, timeoutMs, logger);

  if (incomingMessage == Messages::SCAN_CANCELLED) {
    logger.log("Received stop signal");
    return true;
  }
  else {
    logger.log("Did not receive stop signal");
    return false;
  }
}

void HardwareMessenger::connectToServer(zmqpp::socket& socket, Logger& logger) {
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

std::string HardwareMessenger::getServerIp() {
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
