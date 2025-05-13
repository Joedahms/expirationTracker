#include <glog/logging.h>
#include <iostream>
#include <sqlite3.h>
#include <thread>

#include "../../food_item.h"
#include "display_messenger.h"
#include "fooditem.pb.h"
#include "sql_food.h"

DisplayMessenger* DisplayMessenger::s_instance = nullptr;

/**
 * @param context The zeroMQ context with which to create sockets with
 */
DisplayMessenger::DisplayMessenger(const zmqpp::context& context)
    : logger("display_handler.txt"),
      requestHardwareSocket(context, zmqpp::socket_type::request),
      replySocket(context, zmqpp::socket_type::reply) {
  try {
    this->requestHardwareSocket.connect(ExternalEndpoints::hardwareEndpoint);
    this->replySocket.bind(ExternalEndpoints::displayEndpoint);
  } catch (const zmqpp::exception& e) {
    std::cerr << "ZMQ error when constructing display handler: " << e.what();
    exit(1);
  }
}

/**
 * Handle the success of a scan. Store the identified food item in the database.
 *
 * @param None
 * @return None
 */
FoodItem DisplayMessenger::detectionSuccess() {
  this->logger.log("Item detection succeeded, receiving food item");

  FoodItem foodItem;
  bool foodItemReceived = false;
  while (foodItemReceived == false) {
    foodItemReceived =
        receiveFoodItem(this->replySocket, Messages::AFFIRMATIVE, foodItem);
  }
  this->logger.log("Food item received: ");
  foodItem.logToFile(this->logger);

  return foodItem;
}

EngineState DisplayMessenger::startToHardware() {
  this->logger.log("Scan initialized, sending start signal to hardware");

  std::string hardwareResponse =
      sendMessage(this->requestHardwareSocket, Messages::START_SCAN, this->logger);

  if (hardwareResponse == Messages::AFFIRMATIVE) {
    this->logger.log("Hardware starting scan");
    return EngineState::SCANNING;
  }
  else {
    std::cerr << "Invalid response received from hardware: " << hardwareResponse;
    exit(1);
  }
}

EngineState DisplayMessenger::checkDetectionResults(EngineState currentState) {
  const std::string response = Messages::AFFIRMATIVE;
  const int receiveTimeoutMs = 1;
  std::string visionRequest =
      receiveMessage(this->replySocket, response, receiveTimeoutMs, this->logger);
  if (visionRequest == "null") {
    return currentState;
  }

  if (visionRequest == Messages::ITEM_DETECTION_SUCCEEDED) {
    this->logger.log("New food item received, switching to scan success state");
    return EngineState::SCAN_SUCCESS;
  }
  else if (visionRequest == Messages::ITEM_DETECTION_FAILED) {
    this->logger.log("Item detection failed, switching to scan failure state");
    return EngineState::SCAN_FAILURE;
  }
  else {
    std::cerr << "Invalid request received from vision: " << visionRequest;
    exit(1);
  }
}

void DisplayMessenger::ignoreVision() {
  const std::string response = Messages::AFFIRMATIVE;
  const int receiveTimeoutMs = 1;
  std::string request =
      receiveMessage(this->replySocket, response, receiveTimeoutMs, this->logger);
  if (request == "null" || request == Messages::ITEM_DETECTION_SUCCEEDED ||
      request == Messages::ITEM_DETECTION_FAILED) {
    ;
  }
}
