#ifndef DISPLAY_HANDLER_H
#define DISPLAY_HANDLER_H

#include <sqlite3.h>
#include <zmqpp/zmqpp.hpp>

#include "../../endpoints.h"
#include "../../food_item.h"
#include "../../logger.h"

class DisplayHandler {
public:
  DisplayHandler(zmqpp::context& context, const std::string& engineEndpoint);
  void handle();

private:
  Logger logger;
  ExternalEndpoints externalEndpoints;
  const std::string engineEndpoint;

  zmqpp::socket requestEngineSocket;
  zmqpp::socket requestHardwareSocket;
  zmqpp::socket requestVisionSocket;

  zmqpp::socket replySocket;

  void sendStartSignal();
};

#endif
