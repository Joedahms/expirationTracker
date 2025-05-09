#include "zero.h"

std::string sendMessage(zmqpp::socket& socket,
                        const std::string& message,
                        Logger& logger) {
  logger.log("Sending message: " + message);
  socket.send(message);
  logger.log("Message sent, awaiting response");
  std::string response;
  socket.receive(response);
  logger.log("Response received: " + response);
  return response;
}

std::string receiveMessage(zmqpp::socket& socket,
                           const std::string& response,
                           const int timeoutMs,
                           Logger& logger) {
  try {
    std::string request = "null";
    if (timeoutMs == 0) {
      logger.log("Receiving message with no timeout");
      socket.receive(request, true);
    }
    else {
      logger.log("Receiving message with " + std::to_string(timeoutMs) + " ms timeout");
      zmqpp::poller poller;
      poller.add(socket);

      if (poller.poll(timeoutMs)) {
        if (poller.has_input(socket)) {
          socket.receive(request);
          logger.log("Received message: " + request);
          socket.send(response);
        }
      }
    }

    return request;
  } catch (const zmqpp::exception& e) {
    std::cerr << "zmqpp error when receiving message: " << e.what();
    exit(1);
  }
}
