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
  replySocket.bind(ExternalEndpoints::visionEndpoint);

  // ImageProcessor processor(context);

  zmqpp::poller poller;
  poller.add(replySocket);

  while (1) {
    FoodItem foodItem;
    logger.log("Waiting for start signal from Hardware");

    while (startSignalCheck(replySocket, logger, foodItem, poller) == false) {
      ;
    }
    // processor.setFoodItem(foodItem);
    // processor.process();
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
