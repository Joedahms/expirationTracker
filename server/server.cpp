#include <iostream>

#include "server_messenger.h"

int main(void) {
  zmqpp::context context;
  Logger serverLogger("server.txt");

  ServerMessenger ServerMessenger(context, "../network_config.json", serverLogger);

  return 0;
}
