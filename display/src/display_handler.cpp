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
    if (receivedMessage == Messages::START_SCAN) {
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
          LOG(FATAL)
              << "Received invalid response from hardware after sending start signal";
        }

      } catch (const zmqpp::exception& e) {
        LOG(FATAL) << e.what();
      }

      try {
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

          this->logger.log("Food item received, storing in database");
          sqlite3* database = nullptr;
          openDatabase(&database);
          storeFoodItem(database, foodItem);
          sqlite3_close(database);
          this->logger.log("Food item stored in database");

          this->requestEngineSocket.send(Messages::ITEM_DETECTION_SUCCEEDED);
          std::string engineResponse;
          this->requestEngineSocket.receive(engineResponse);
          if (engineResponse == Messages::AFFIRMATIVE) {
            this->logger.log("Item stored in database and engine notified of success");
          }
          else {
            LOG(FATAL) << "Unexpected message received";
          }
        }
        else {
          LOG(FATAL) << "Unexpected message received";
        }
      } catch (const zmqpp::exception& e) {
        LOG(FATAL) << e.what();
      }
    }
  }
}
