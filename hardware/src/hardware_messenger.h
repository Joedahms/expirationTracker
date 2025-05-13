#ifndef HARDWARE_MESSENGER_H
#define HARDWARE_MESSENGER_H

#include "../../messenger.h"

class HardwareMessenger : public Messenger {
public:
  bool checkStartSignal(zmqpp::socket& socket, const int timeoutMs, Logger& logger);
};

#endif
