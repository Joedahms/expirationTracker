#ifndef ENGINE_STATE_H
#define ENGINE_STATE_H

#include <string>

enum class EngineState {
  SCANNING,
  ITEM_LIST,
  CANCEL_SCAN_CONFIRMATION,
  SCAN_SUCCESS,
  SCAN_FAILURE
};

std::string engineStateToString(EngineState engineState);

#endif
