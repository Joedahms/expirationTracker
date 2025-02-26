#ifndef ENGINE_STATE_H
#define ENGINE_STATE_H

#include <string>

enum class EngineState { MAIN_MENU, SCANNING, PAUSE_MENU, ITEM_LIST };

std::string engineStateToString(EngineState engineState);

#endif
