#include <glog/logging.h>
#include <iostream>
#include <sqlite3.h>
#include <thread>

#include "../../food_item.h"
#include "display_handler.h"
#include "fooditem.pb.h"
#include "sql_food.h"

/**
 * @param context The zeroMQ context with which to create sockets with
 * @param engineEndpoint zeroMQ endpoint to communicate with the display engine
 */
DisplayHandler::DisplayHandler(zmqpp::context& context, const std::string& engineEndpoint)
    : engineEndpoint(engineEndpoint),
      requestEngineSocket(context, zmqpp::socket_type::request),
      requestHardwareSocket(context, zmqpp::socket_type::request),
      requestVisionSocket(context, zmqpp::socket_type::request),
      replySocket(context, zmqpp::socket_type::reply), logger("display_handler.txt") {
  try {
    this->requestEngineSocket.connect(engineEndpoint);
    this->requestHardwareSocket.connect(ExternalEndpoints::hardwareEndpoint);
    this->requestVisionSocket.connect(ExternalEndpoints::visionEndpoint);
    this->replySocket.bind(ExternalEndpoints::displayEndpoint);
  } catch (const zmqpp::exception& e) {
    LOG(FATAL) << e.what();
  }
}

/**
 * Handle all communiation into and out of the display. Check for start signal from
 * engine, send start signal to hardware, then wait for message from vision.
 *
 * @param None
 * @return None
 */
void DisplayHandler::handle() {
  while (true) {
    std::string receivedMessage;
    this->replySocket.receive(receivedMessage);
    this->logger.log("received message from socket: " + receivedMessage);
    // Engine wants to start a new scan
    if (receivedMessage == Messages::START_SCAN) {
      startSignalToHardware();
      receiveFromVision();
    }
    else {
      LOG(FATAL) << "Unexpected message received";
    }
  }
}

/**
 * Indicate to hardware that the user has placed an item on the platform and would like to
 * scan it.
 *
 * @param None
 * @return None
 */
void DisplayHandler::startSignalToHardware() {
  this->logger.log("Received start scan from engine");
  try {
    this->logger.log("Telling hardware to start scan");
    this->replySocket.send(Messages::AFFIRMATIVE);
    this->requestHardwareSocket.send(Messages::START_SCAN);

    std::string hardwareResponse;
    this->requestHardwareSocket.receive(hardwareResponse);
    if (hardwareResponse == Messages::AFFIRMATIVE) {
      this->logger.log("Hardware starting scan");
    }
    else {
      LOG(FATAL) << "Received invalid response from hardware after sending start signal";
    }
  } catch (const zmqpp::exception& e) {
    LOG(FATAL) << e.what();
  }
}

/**
 * Wait for a response from vision indicating success of scan.
 *
 * @param None
 * @return None
 */
void DisplayHandler::receiveFromVision() {
  this->logger.log("Waiting for message from vision");
  std::string request;
  this->replySocket.receive(request);
  if (request == Messages::ITEM_DETECTION_FAILED) {
    detectionFailure();
  }
  else if (request == Messages::ITEM_DETECTION_SUCCEEDED) {
    detectionSuccess();
  }
  else {
    LOG(FATAL) << "Unexpected message received";
  }
}

/**
 * Handle the failure of a scan.
 *
 * @param None
 * @return None
 */
void DisplayHandler::detectionFailure() {
  this->replySocket.send(Messages::AFFIRMATIVE);
  this->logger.log("Item detection failed");
  // TODO Display failure message
}

/**
 * Handle the success of a scan. Store the identified food item in the database and
 * indicate to the engine that the scan was successful.
 *
 * @param None
 * @return None
 */
void DisplayHandler::detectionSuccess() {
  this->replySocket.send(Messages::AFFIRMATIVE);
  this->logger.log("Item detection succeeded, receiving food item");
  FoodItem foodItem;
  receiveFoodItem(this->replySocket, Messages::AFFIRMATIVE, foodItem);

  this->logger.log("Food item received, storing in database");
  sqlite3* database = nullptr;
  openDatabase(&database);
  storeFoodItem(database, foodItem);
  sqlite3_close(database);
  this->logger.log("Food item stored in database");

  this->logger.log("Sending success message to engine");
  this->requestEngineSocket.send(Messages::ITEM_DETECTION_SUCCEEDED);
  this->logger.log("Success message send to engine");
  std::string engineResponse;
  this->logger.log("Receiving response from engine");
  this->requestEngineSocket.receive(engineResponse);
  this->logger.log("Received response from engine");
  if (engineResponse == Messages::AFFIRMATIVE) {
    this->logger.log("Item stored in database and engine notified of success");
  }
  else {
    LOG(FATAL) << "Unexpected message received";
  }
}
