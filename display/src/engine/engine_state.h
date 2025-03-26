#ifndef ENGINE_STATE_H
#define ENGINE_STATE_H

#include <string>

enum class EngineState { SCANNING, ITEM_LIST, ZERO_WEIGHT, MAIN_MENU };

std::string engineStateToString(EngineState engineState);

#endif
