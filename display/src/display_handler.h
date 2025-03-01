#ifndef DISPLAY_HANDLER_H
#define DISPLAY_HANDLER_H

#include <sqlite3.h>
#include <zmqpp/zmqpp.hpp>

#include "../../food_item.h"
#include "../../pipes.h"

class DisplayHandler {
public:
  void handleExternal();
  void handleEngine();

private:
  //  zmqpp::socket replySocket()

  void sendStartSignal();
};

#endif
