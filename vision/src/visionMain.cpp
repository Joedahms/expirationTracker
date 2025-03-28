#include "../include/visionMain.h"

/**
 * Entry into the vision code. Only called from main after vision child process is
 * forked.
 *
 * @param context The zeroMQ context with which to create sockets with
 * @param externalEndpoints Endpoints to the main components of the system (vision,
 * hardware, and display)
 * @return None
 */
void visionEntry(zmqpp::context& context) {
  Logger logger("vision_entry.txt");
  logger.log("Within vision process");

  zmqpp::socket replySocket(context, zmqpp::socket_type::reply);
  replySocket.bind(VisionExternalEndpoints::visionMainEndpoint);

  zmqpp::socket listenerSocket(context, zmqpp::socket_type::reply);
  listenerSocket.bind(ExternalEndpoints::visionEndpoint);
  zmqpp::socket messengerSocket(context, zmqpp::socket_type::request);
  messengerSocket.connect(VisionExternalEndpoints::visionMainEndpoint);

  ImageProcessor processor(context);

  // Listener thread
  std::thread listenerThread([&]() {
    while (true) {
      zmqpp::message msg;
      listenerSocket.receive(msg);
      std::string command;
      msg >> command;
      if (command == Messages::SCAN_CANCELLED) {
        logger.log("Cancel command received.");
        processor.requestCancel();
        listenerSocket.send(Messages::AFFIRMATIVE);
      }
      else if (command == Messages::START_SCAN) {
        logger.log("Start command received.");
        listenerSocket.send(Messages::AFFIRMATIVE); // Tell hardware to send food item
        zmqpp::message foodItemString;
        listenerSocket.receive(foodItemString);     // Grab food item
        listenerSocket.send(Messages::AFFIRMATIVE); // Tell hardware received food itme

        messengerSocket.send(foodItemString); // Send fooditem to main
        zmqpp::message response;
        messengerSocket.receive(response); // Receive back affirmative
      }
    }
  });
  listenerThread.detach();

  zmqpp::poller poller;
  poller.add(replySocket);

  while (1) {
    FoodItem foodItem;
    logger.log("Waiting for start signal from Hardware");

    while (!startSignalCheck(replySocket, logger, foodItem, poller)) {
      ;
    }
    processor.setFoodItem(foodItem);
    processor.process();

    if (processor.isCancelRequested()) {
      logger.log("Process was canceled by user.");
    }
    processor.resetCancel();
  }
}

/**
 * polls for 1000ms for socket activity and then checks replySocket for input
 *
 * @param replySocket replySocket to check for input on
 * @param logger Logger being used to log visionMain
 * @param foodItem foodItem to return with data
 * @param poller poller to use for polling socket activity
 * @return whether start signal was received or not
 */
bool startSignalCheck(zmqpp::socket& replySocket,
                      const Logger& logger,
                      FoodItem& foodItem,
                      zmqpp::poller& poller) {
  try {
    if (poller.poll(1000)) {
      if (poller.has_input(replySocket)) {
        logger.log("Start signal check has input.");
        receiveFoodItem(replySocket, Messages::AFFIRMATIVE, foodItem);
        logger.log("Received start signal from hardware: ");
        foodItem.logToFile(logger);
        return true;
      }
      return false;
    }
    else {
      logger.log("Did not receive start signal from hardware");
      return false;
    }
  } catch (const zmqpp::exception& e) {
    LOG(FATAL) << e.what();
    return false;
  }
}
