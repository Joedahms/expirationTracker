#ifndef PAUSE_MENU_H
#define PAUSE_MENU_H

#include <SDL2/SDL_ttf.h>
#include <memory>
#include <string>

#include "../button.h"
#include "../display_global.h"
#include "../text.h"

/**
 * Pause menu state. Entered when pausing from gameplay.
 */
class PauseMenu {
public:
  PauseMenu(struct DisplayGlobal);
  int handleEvents(bool*);
  void render();

private:
  struct DisplayGlobal displayGlobal;
  std::unique_ptr<Text> title;
  std::unique_ptr<Button> resumeButton;
  std::unique_ptr<Button> mainMenuButton;
};

#endif
