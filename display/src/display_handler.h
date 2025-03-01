#ifndef DISPLAY_HANDLER_H
#define DISPLAY_HANDLER_H

#include <sqlite3.h>
#include <zmqpp/zmqpp.hpp>

#include "../../food_item.h"
#include "../../pipes.h"

class DisplayHandler {
public:
  DisplayHandler(struct Endpoints endpoints, const std::string engineEndpoint);
  void handleExternal();

  void handleEngine();

private:
  struct Endpoints endpoints;
  const std::string engineEndpoint;

  void sendStartSignal();
};

#endif
