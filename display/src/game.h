#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <memory>
#include <string>
#include <vector>

#include "gameplay.h"
#include "main_menu.h"
#include "pause_menu.h"

#include "game_global.h"

class Game {
public:
  Game(const char*, int, int, int, int, bool, struct GameGlobal);

  SDL_Window* setupWindow(
      const char*, int, int, int, int, bool); // Setup the SDL game window
  void initializeSdl(SDL_Window*);

  void checkState();
  void handleEvents();   // Handle events depending on current state
  void checkKeystates(); // Check keyboard key presses depending on state
  void update();         // Update game depending on state

  void renderState();
  void clean(); // Clean up upon quit
  bool running() { return gameIsRunning; }

private:
  struct GameGlobal gameGlobal;

  // State the game is currently in
  // 0: Main menu
  // 1: Gameplay
  // 2: Pause menu
  int state = 0;

  // States
  std::unique_ptr<MainMenu> mainMenu; // State entered when the game starts
  std::unique_ptr<Gameplay> gameplay;
  std::unique_ptr<PauseMenu> pauseMenu;

  bool gameIsRunning = false;
};

#endif
