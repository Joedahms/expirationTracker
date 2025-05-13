#ifndef DISPLAY_HANDLER_H
#define DISPLAY_HANDLER_H

#include <sqlite3.h>
#include <zmqpp/zmqpp.hpp>

#include "../../endpoints.h"
#include "../../food_item.h"
#include "../../logger.h"
#include "../../messenger.h"
#include "engine_state.h"

#include <iostream>

class DisplayMessenger : public Messenger {
public:
  static DisplayMessenger& getInstance() {
    if (s_instance == nullptr) {
      static zmqpp::context default_context;
      static DisplayMessenger default_instance(default_context);
      s_instance = &default_instance;
    }
    return *s_instance;
  }

  static void init(const zmqpp::context& context) {
    static DisplayMessenger instance(context);
    s_instance = &instance;
  }

  FoodItem detectionSuccess();
  EngineState startToHardware();
  EngineState checkDetectionResults(EngineState currentState);
  void ignoreVision();

private:
  DisplayMessenger();
  DisplayMessenger(const zmqpp::context& context);

  Logger logger;

  zmqpp::socket requestHardwareSocket;
  zmqpp::socket replySocket;

  static DisplayMessenger* s_instance;

  // Singleton
  DisplayMessenger(const DisplayMessenger&)            = delete;
  DisplayMessenger& operator=(const DisplayMessenger&) = delete;
  DisplayMessenger(DisplayMessenger&&)                 = delete;
  DisplayMessenger& operator=(DisplayMessenger&&)      = delete;
};

#endif
