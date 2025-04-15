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
 */
DisplayHandler::DisplayHandler(const zmqpp::context& context)
    : logger("display_handler.txt"),
      requestHardwareSocket(context, zmqpp::socket_type::request),
      requestVisionSocket(context, zmqpp::socket_type::request),
      replySocket(context, zmqpp::socket_type::reply) {
  try {
    this->requestHardwareSocket.connect(ExternalEndpoints::hardwareEndpoint);
    this->requestVisionSocket.connect(ExternalEndpoints::visionEndpoint);
    this->replySocket.bind(ExternalEndpoints::displayEndpoint);
  } catch (const zmqpp::exception& e) {
    LOG(FATAL) << e.what();
  }
}

/**
 * Send a message to a zeroMQ endpoint and get back its response.
 *
 * @param message The message to send to the endpoint
 * @param endpoint The zeroMQ endpoint to send the message to
 * @return The response from the endpoint after sending it the message
 */
std::string DisplayHandler::sendMessage(const std::string& message,
                                        const std::string& endpoint) {
  this->logger.log("Entering sendMessage");
  try {
    std::string response;

    if (endpoint == ExternalEndpoints::hardwareEndpoint) {
      this->logger.log("Sending message to hardware: " + message);
      this->requestHardwareSocket.send(message);
      this->logger.log("Message sent to hardware, awaiting response");
      this->requestHardwareSocket.receive(response);
      this->logger.log("Response received: " + response);
      return response;
    }
    else if (endpoint == ExternalEndpoints::visionEndpoint) {
      this->logger.log("Sending message to vision: " + message);
      this->requestVisionSocket.send(message);
      this->logger.log("Message sent to vision, awaiting response");
      this->requestVisionSocket.receive(response);
      this->logger.log("Response received: " + response);
      return response;
    }
    else {
      LOG(FATAL) << "Attempt to send message to invalid endpoint";
    }
  } catch (const zmqpp::exception& e) {
    LOG(FATAL) << "zmqpp error when sending message: " << e.what();
  }
}

/**
 * Receive a zeroMQ message with a specified timeout and send back response if a message
 * is received.
 *
 * @param response What to send back to requesting socket upon receiving a message.
 * @param timeoutMS Max amount of time to wait for a message. If set to 0, return right
 * away if no message.
 * @return The message received. Returns "null" if no message received.
 */
std::string DisplayHandler::receiveMessage(const std::string& response,
                                           const int timeoutMS) {
  try {
    std::string request = "null";
    if (timeoutMS == 0) {
      this->logger.log("Receiving message with no timeout");
      this->replySocket.receive(request, true);
    }
    else {
      this->logger.log("Receiving message with " + std::to_string(timeoutMS) +
                       " ms timeout");
      zmqpp::poller poller;
      poller.add(this->replySocket);

      if (poller.poll(timeoutMS)) {
        if (poller.has_input(this->replySocket)) {
          this->replySocket.receive(request);
          this->logger.log("Received message: " + request);
          this->replySocket.send(response);
        }
      }
    }

    return request;
  } catch (const zmqpp::exception& e) {
    LOG(FATAL) << "zmqpp error when receiving message: " << e.what();
  }
}

/**
 * Handle the success of a scan. Store the identified food item in the database.
 *
 * @param None
 * @return None
 */
void DisplayHandler::detectionSuccess() {
  this->logger.log("Item detection succeeded, receiving food item");

  FoodItem foodItem;
  bool foodItemReceived = false;
  while (foodItemReceived == false) {
    foodItemReceived =
        receiveFoodItem(this->replySocket, Messages::AFFIRMATIVE, foodItem);
  }

  this->logger.log("Food item received: ");
  foodItem.logToFile(this->logger);

  this->logger.log("Storing food item in database");
  sqlite3* database = nullptr;
  openDatabase(&database);
  storeFoodItem(database, foodItem);
  sqlite3_close(database);
  this->logger.log("Food item stored in database");
}
