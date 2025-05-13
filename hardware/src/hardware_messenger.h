#ifndef HARDWARE_MESSENGER_H
#define HARDWARE_MESSENGER_H

#include "../../messenger.h"

class HardwareMessenger : public Messenger {
public:
  HardwareMessenger(zmqpp::context& context,
                    const std::string& configFilename,
                    Logger& logger);

  bool checkStartSignal(const int timeoutMs, Logger& logger);
  bool checkStopSignal(const int timeoutMs, Logger& logger);

private:
  zmqpp::socket requestServerSocket;
  zmqpp::socket requestDisplaySocket;
  zmqpp::socket replySocket;

  void connectToServer(zmqpp::socket& socket, Logger& logger);
  std::string getServerIp();

  int serverPort    = -1;
  int discoveryPort = -1;
};

#endif
