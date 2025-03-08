#include "../include/ModelHandler.h"
#include <arpa/inet.h>

#define BROADCAST_IP   "255.255.255.255"
#define zeroMQPort     5555
#define DISCOVERY_PORT 5005
ModelHandler::ModelHandler(zmqpp::context& context)
    : logger("model_handler.txt"),
      textClassifier(context, VisionExternalEndpoints::textClassifierEndpoint) {
  std::string serverIP = this->discoverServerViaUDP();
  if (!serverIP.empty()) {
    std::cout << "Discovered Server IP: " << serverIP << std::endl;
    std::string serverAddress = "tcp://" + serverIP + ":" + std::to_string(zeroMQPort);
    // Now use this IP for ZeroMQ communication

    this->textClassifier.connectToServer(serverAddress);
  }
  else {
    std::cout << "Failed to discover server." << std::endl;
  }
}

std::string ModelHandler::discoverServerViaUDP() {
  int sockfd;
  struct sockaddr_in serverAddr;
  socklen_t addrLen = sizeof(serverAddr);
  char buffer[1024] = {0};

  // Create UDP socket
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    LOG(FATAL) << ("UDP socket creation failed");
    return "";
  }

  // Enable broadcast mode
  int broadcastEnable = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));

  // Set destination address
  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family      = AF_INET;
  serverAddr.sin_addr.s_addr = inet_addr(BROADCAST_IP);
  serverAddr.sin_port        = htons(DISCOVERY_PORT);

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

/**
 * Run text extraction to attempt to identify the item. Upon success, update the given
 * FoodItem
 *
 * Input:
 * @param imagePath Path to the image to extract text from
 * @param foodItem foodItem to update
 * @return Whether classification was successful or not
 */
bool ModelHandler::classifyObject(const std::filesystem::path& imagePath,
                                  FoodItem& foodItem) {
  this->logger.log("Running text classificiation");
  auto result = this->textClassifier.handleClassification(imagePath);
  this->logger.log("Text classification complete");

  // update foodItem
  if (result) {
    foodItem.setName(result.value());
    foodItem.setImagePath(std::filesystem::absolute(imagePath));
    foodItem.setCategory(FoodCategories::packaged);
    foodItem.setQuantity(1);
    foodItem.setExpirationDate(std::chrono::year_month_day{
        std::chrono::year{2025}, std::chrono::month{3}, std::chrono::day{15}});
    return true;
  }
  return false;
}
