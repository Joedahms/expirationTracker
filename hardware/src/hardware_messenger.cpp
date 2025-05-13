#include "../../endpoints.h"
#include "hardware_messenger.h"

/**
 * Checks for a start signal from the display. Return if have not received a message by
 * the timeout.
 *
 * @param None
 * @return bool - True if start signal received, false otherwise
 */
bool HardwareMessenger::checkStartSignal(zmqpp::socket& socket,
                                         const int timeoutMs,
                                         Logger& logger) {
  logger.log("Checking for start signal from display");

  std::string incomingMessage;
  incomingMessage = receiveMessage(socket, Messages::AFFIRMATIVE, timeoutMs, logger);

  if (incomingMessage == Messages::START_SCAN) {
    logger.log("Received start signal from display");
    return true;
  }
  else {
    logger.log("Did not receive start signal from display");
    return false;
  }
}
