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

  this->serverAddress = config["network"]["serverAddress"];

  try {
    this->requestServerSocket.connect(this->serverAddress);
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
