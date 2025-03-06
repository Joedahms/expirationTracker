#include <glog/logging.h>
#include <iostream>
#include <sqlite3.h>
#include <thread>

#include "../../food_item.h"
#include "display_handler.h"
#include "fooditem.pb.h"
#include "sql_food.h"

DisplayHandler::DisplayHandler(zmqpp::context& context, const std::string& engineEndpoint)
    : engineEndpoint(engineEndpoint),
      requestEngineSocket(context, zmqpp::socket_type::request),
      requestHardwareSocket(context, zmqpp::socket_type::request),
      requestVisionSocket(context, zmqpp::socket_type::request),
      replySocket(context, zmqpp::socket_type::reply), logger("display_handler.txt") {
  try {
    this->requestEngineSocket.connect(this->engineEndpoint);
    this->requestHardwareSocket.connect(ExternalEndpoints::hardwareEndpoint);
    this->requestVisionSocket.connect(ExternalEndpoints::visionEndpoint);
    this->replySocket.bind(ExternalEndpoints::displayEndpoint);
  } catch (const zmqpp::exception& e) {
    LOG(FATAL) << e.what();
  }
}

void DisplayHandler::handle() {
  while (true) {
    std::string receivedMessage;
    this->replySocket.receive(receivedMessage);

    // Engine wants to start a new scan
    if (receivedMessage == "start scan") {
      this->logger.log("Received start scan");
      try {
        this->logger.log("Telling hardware to start scan");

        this->replySocket.send(receivedMessage);
        this->requestHardwareSocket.send(Messages::START_SCAN);
        std::string hardwareResponse;
        this->requestHardwareSocket.receive(hardwareResponse);
        if (hardwareResponse == Messages::AFFIRMATIVE) {
          this->logger.log("Hardware starting scan");
        }
        else {
          LOG(FATAL)
              << "Received invalid response from hardware after sending start signal";
        }

      } catch (const zmqpp::exception& e) {
        LOG(FATAL) << e.what();
      }

      this->logger.log("Waiting for message from vision");
      std::string request;
      this->replySocket.receive(request);
      if (request == Messages::ITEM_DETECTION_FAILED) {
        this->logger.log("Item detection failed");
        // TODO Display failure message
      }
      else if (request == Messages::ITEM_DETECTION_SUCCEEDED) {
        this->replySocket.send(Messages::AFFIRMATIVE);
        this->logger.log("Item detection succeeded, receiving food item");
        FoodItem foodItem;
        receiveFoodItem(this->replySocket, Messages::AFFIRMATIVE, foodItem);
        this->logger.log("Food item received");
      }
      else {
        LOG(FATAL) << "Unexpected message received";
      }
    }
  }
}

/**
 * Handle communication between display and the other major components of the system
 * (vision and hardware).
 *
 * @param None
 * @return None
 */
/*
void DisplayHandler::handleExternal() {
  struct timeval timeout;
  timeout.tv_sec  = 1;
  timeout.tv_usec = 0;

  struct FoodItem foodItem;
  bool foodItemReceived = false;
  foodItemReceived =
      receiveFoodItem(foodItem, this->externalPipes.visionToDisplay[READ], timeout);

  if (foodItemReceived) {
    LOG(INFO) << "Food item received from vision";
    sqlite3* database = nullptr;
    openDatabase(&database);

    storeFoodItem(database, foodItem);
    writeString(this->displayToEngine[WRITE], "ID successful");

    sqlite3_close(database);
  }
}

*/
/**
 * Handle communication between the main display process and the engine process.
 *
 * @param None
 * @param None
 */
/*
void DisplayHandler::handleEngine() {
  fd_set readPipeSet;

  FD_ZERO(&readPipeSet);
  FD_SET(this->engineToDisplay[READ], &readPipeSet);

  struct timeval timeout;
  timeout.tv_sec  = 1;
  timeout.tv_usec = 0;
  int pipeReady =
      select(this->engineToDisplay[READ] + 1, &readPipeSet, NULL, NULL, &timeout);

  if (pipeReady == -1) {
    LOG(FATAL) << "Select error when receiving food item";
  }
  else if (pipeReady == 0) { // No data available
    ;
  }
  if (FD_ISSET(this->engineToDisplay[READ], &readPipeSet)) { // Data available
    sendStartSignal();
  }
}
*/
/**
 * Signify to hardware that the user would like to scan in an item.
 *
 * @param None
 * @return None
 */
/*
void DisplayHandler::sendStartSignal() {
  LOG(INFO) << "Sending start signal to hardware";
  std::string sdlString = readString(this->engineToDisplay[READ]);
  writeString(this->externalPipes.displayToHardware[WRITE], sdlString);
}
*/
