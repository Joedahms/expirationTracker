#include "engine_state.h"

std::string engineStateToString(EngineState engineState) {
  switch (engineState) {
  case EngineState::SCANNING:
    return "SCANNING";
  case EngineState::ITEM_LIST:
    return "ITEM_LIST";
  case EngineState::CANCEL_SCAN_CONFIRMATION:
    return "CANCEL_SCAN_CONFIRMATION";
  case EngineState::SCAN_SUCCESS:
    return "SCAN_SUCCESS";
  case EngineState::SCAN_FAILURE:
    return "SCAN_FAILURE";
  default:
    return "UNKNOWN";
  }
}
