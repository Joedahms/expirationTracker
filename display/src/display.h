#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <memory>
#include <string>
#include <vector>

#include "main_menu.h"
#include "pause_menu.h"
#include "scanning.h"

#include "display_global.h"

class Display {
public:
  Display(const char*, int, int, int, int, bool, struct DisplayGlobal);

  SDL_Window* setupWindow(
      const char*, int, int, int, int, bool); // Setup the SDL display window
  void initializeSdl(SDL_Window*);

  void checkState();
  void handleEvents();   // Handle events depending on current state
  void checkKeystates(); // Check keyboard key presses depending on state
  void update();         // Update display depending on state

  void renderState();
  void clean(); // Clean up upon quit
  bool running() { return displayIsRunning; }

private:
  struct DisplayGlobal displayGlobal;

  // State the display is currently in
  // 0: Main menu
  // 1: Scanning
  // 2: Pause menu
  int state = 0;

  // States
  std::unique_ptr<MainMenu> mainMenu; // State entered when the display boots
  std::unique_ptr<Scanning> scanning; // State entered when an item is being scanned
  std::unique_ptr<PauseMenu> pauseMenu;

  bool displayIsRunning = false;
};

#endif
