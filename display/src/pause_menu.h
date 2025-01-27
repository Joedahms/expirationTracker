#ifndef PAUSE_MENU_H
#define PAUSE_MENU_H

#include <SDL2/SDL_ttf.h>
#include <memory>
#include <string>

#include "button.h"
#include "game_global.h"
#include "text.h"

/**
 * @class PauseMenu
 *
 * Pause menu state. Entered when pausing from gameplay.
 */
class PauseMenu {
public:
  PauseMenu(struct GameGlobal);
  int handleEvents(bool*);
  void render();

private:
  struct GameGlobal gameGlobal;
  std::unique_ptr<Text> title;
  std::unique_ptr<Button> resumeButton;
  std::unique_ptr<Button> mainMenuButton;
};

#endif
