#ifndef DISPLAY_ENGINE_H
#define DISPLAY_ENGINE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <memory>
#include <string>
#include <vector>

#include "states/item_list.h"
#include "states/main_menu.h"
#include "states/pause_menu.h"
#include "states/scanning.h"

#include "display_global.h"
#include "engine_state.h"

class DisplayEngine {
public:
  DisplayEngine(const char* windowTitle,
                int windowXPosition,
                int windowYPosition,
                int screenWidth,
                int screenHeight,
                bool fullscreen,
                struct DisplayGlobal displayGlobal);

  SDL_Window* setupWindow(const char* windowTitle,
                          int windowXPosition,
                          int windowYPosition,
                          int screenWidth,
                          int screenHeight,
                          bool fullscreen);
  void initializeEngine(SDL_Window* window);

  void checkState();
  void handleEvents(int* engineToDisplay, int* displayToEngine);
  void checkKeystates();
  void update();

  void renderState();
  void clean(); // Clean up upon quit
  bool running() { return displayIsRunning; }

private:
  struct DisplayGlobal displayGlobal;

  // State the display is currently in
  EngineState engineState = EngineState::MAIN_MENU;

  // States
  std::unique_ptr<MainMenu> mainMenu; // State entered when the display boots
  std::unique_ptr<Scanning> scanning; // State entered when an item is being scanned
  std::unique_ptr<PauseMenu> pauseMenu;
  std::unique_ptr<ItemList> itemList; // Display all items stored

  bool displayIsRunning = false;
};

#endif
