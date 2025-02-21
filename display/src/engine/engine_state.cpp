#include "engine_state.h"

std::string engineStateToString(EngineState engineState) {
  switch (engineState) {
  case EngineState::MAIN_MENU:
    return "MAIN_MENU";
  case EngineState::SCANNING:
    return "SCANNING";
  case EngineState::PAUSE_MENU:
    return "PAUSE_MENU";
  case EngineState::ITEM_LIST:
    return "ITEM_LIST";
  default:
    return "UNKNOWN";
  }
}
