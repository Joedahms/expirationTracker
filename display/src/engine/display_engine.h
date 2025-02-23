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

/**
 * Class containing all objects related to the engine.
 */
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
  void clean();
  bool running() { return displayIsRunning; }

private:
  struct DisplayGlobal displayGlobal;

  EngineState engineState = EngineState::MAIN_MENU;

  // States
  std::unique_ptr<MainMenu> mainMenu;
  std::unique_ptr<Scanning> scanning;
  std::unique_ptr<PauseMenu> pauseMenu;
  std::unique_ptr<ItemList> itemList;

  bool displayIsRunning = false;
};

#endif
