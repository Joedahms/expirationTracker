#ifndef DISPLAY_HANDLER_H
#define DISPLAY_HANDLER_H

#include <sqlite3.h>
#include <zmqpp/zmqpp.hpp>

#include "../../food_item.h"
#include "../../pipes.h"

class DisplayHandler {
public:
  DisplayHandler(zmqpp::context& context,
                 struct ExternalEndpoints externalEndpoints,
                 const std::string& engineEndpoint);
  void handle();

private:
  ExternalEndpoints externalEndpoints;
  const std::string engineEndpoint;

  zmqpp::socket requestEngineSocket;
  zmqpp::socket replyEngineSocket;

  zmqpp::socket requestHardwareSocket;
  zmqpp::socket replyHardwareSocket;

  zmqpp::socket requestVisionSocket;
  zmqpp::socket replyVisionSocket;

  void sendStartSignal();
};

#endif
