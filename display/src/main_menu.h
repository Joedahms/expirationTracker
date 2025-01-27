#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <SDL2/SDL_ttf.h>
#include <memory>
#include <string>

#include "button.h"
#include "game_global.h"
#include "text.h"

/**
 * @class MainMenu
 *
 * Main menu state. This is the state the game enters upon launching.
 */
class MainMenu {
public:
  MainMenu(struct GameGlobal);
  int handleEvents(bool*); // Handle SDL events while in the main menu state
  void render();

private:
  struct GameGlobal gameGlobal;
  std::unique_ptr<Text> title;         // Title of the game
  std::unique_ptr<Button> startButton; // Launch the game
};

#endif
