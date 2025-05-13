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

void HardwareMessenger::sendPhotos(Logger& logger,
                                   const std::filesystem::path& imageDirectory,
                                   const int angle) {
  logger.log("Sending angle " + std::to_string(angle) + " photos");

  std::filesystem::path topImagePath =
      imageDirectory / (std::to_string(angle) + "_top.jpg");
  std::filesystem::path sideImagePath =
      imageDirectory / (std::to_string(angle) + "_side.jpg");

  const std::string topImagePathString  = topImagePath.string();
  const std::string sideImagePathString = sideImagePath.string();

  logger.log("Looking for image: " + topImagePathString);
  logger.log("Looking for image: " + sideImagePathString);

  bool topExists  = false;
  bool sideExists = false;

  while (!(topExists && sideExists)) { // Wait until BOTH images exist
    topExists  = std::filesystem::exists(topImagePath);
    sideExists = std::filesystem::exists(sideImagePath);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  std::ifstream topImage(topImagePath, std::ios::binary | std::ios::ate);
  if (!topImage) {
    std::cerr << "Failed to open file: " << topImagePath << std::endl;
    exit(1);
  }

  std::streamsize topImageSize = topImage.tellg();
  topImage.seekg(0, std::ios::beg);

  std::vector<char> buffer(topImageSize);
  if (!topImage.read(buffer.data(), topImageSize)) {
    std::cerr << "Failed to read file" << std::endl;
    exit(1);
  }

  // zmqpp::message message(buffer.data(), buffer.size());
  std::string message(buffer.data(), buffer.size());
  sendMessage(this->requestServerSocket, message, logger);

  std::string response = receiveMessage(this->replySocket, "yuh", 1000, logger);
}
