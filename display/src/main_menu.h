#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <SDL2/SDL_ttf.h>
#include <memory>
#include <string>

#include "button.h"
#include "display_global.h"
#include "text.h"

/**
 * Main menu state. This is the state the display enters upon launching.
 */
class MainMenu {
public:
  MainMenu(struct DisplayGlobal);
  int handleEvents(bool*); // Handle SDL events while in the main menu state
  void render();

private:
  struct DisplayGlobal displayGlobal;
  std::unique_ptr<Text> title;         // Title of the display
  std::unique_ptr<Button> startButton; // Launch the display
};

#endif
