#include <iostream>

#include "server_messenger.h"

int main(void) {
  zmqpp::context context;
  Logger serverLogger("server.txt");

  ServerMessenger serverMessenger(context, "../network_config.json", serverLogger);

  while (1) {
    serverMessenger.receivePhoto(serverLogger);
  }

  return 0;
}
