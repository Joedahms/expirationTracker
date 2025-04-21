#ifndef SCANNING_H
#define SCANNING_H

#include <SDL2/SDL.h>
#include <memory>
#include <string>

#include "../display_global.h"
#include "../engine_state.h"
#include "state.h"

/**
 * Scanning state. State entered when user presses scan new item button in main menu.
 */
class Scanning : public State {
public:
  Scanning(const struct DisplayGlobal& displayGlobal, const EngineState& state);
  void handleEvents(bool* displayIsRunning) override;
  void render() const override;
  void exit() override;
};

#endif
