#ifndef PAUSE_MENU_H
#define PAUSE_MENU_H

#include <SDL2/SDL_ttf.h>
#include <memory>
#include <string>

#include "../button.h"
#include "../display_global.h"
#include "../text.h"
#include "state.h"

/**
 * Pause menu state. Entered when pausing from gameplay.
 */
class PauseMenu : public State {
public:
  PauseMenu(struct DisplayGlobal);
  int handleEvents(bool*) override;
  void render() const override;
};

#endif
