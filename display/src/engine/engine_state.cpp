#include "engine_state.h"

std::string engineStateToString(EngineState engineState) {
  switch (engineState) {
  case EngineState::SCANNING:
    return "SCANNING";
  case EngineState::ITEM_LIST:
    return "ITEM_LIST";
  case EngineState::ZERO_WEIGHT:
    return "ZERO_WEIGHT";
  default:
    return "UNKNOWN";
  }
}
