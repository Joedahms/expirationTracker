#ifndef SERVER_MESSENGER_H
#define SERVER_MESSENGER_H

#include "../messenger.h"

class ServerMessenger : public Messenger {
public:
  ServerMessenger(zmqpp::context& context,
                  const std::string& configFilename,
                  Logger& logger);

  void receivePhoto(Logger& logger);

private:
  zmqpp::socket replySocket;
};

#endif
