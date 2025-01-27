#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>

#include "game_global.h"
#include "main_menu.h"
#include "rectangle.h"

/**
 * @constructor: MainMenu
 *
 * Setup the game title and start button.
 * @param gameGlobal - Global game variables
 * @output - None
 */
MainMenu::MainMenu(struct GameGlobal gameGlobal) {
  this->gameGlobal           = gameGlobal;
  SDL_Surface* windowSurface = SDL_GetWindowSurface(this->gameGlobal.window);

  // Title
  const char* fontPath     = "../16020_FUTURAM.ttf";
  const char* titleContent = "TRASHORE";
  SDL_Color titleColor     = {0, 255, 0, 255}; // Green

  // x,y -> 100, 100. 0 width/height
  SDL_Rect titleRectangle = {
      100,
      100,
      0,
      0,
  };
  this->title = std::make_unique<Text>(this->gameGlobal, fontPath, titleContent, 24,
                                       titleColor, titleRectangle);
  this->title->centerHorizontal(windowSurface);

  // Start button
  SDL_Rect startButtonRectangle = {200, 150, 200, 50};
  startButtonRectangle = centerRectangleHorizontal(windowSurface, startButtonRectangle);
  this->startButton =
      std::make_unique<Button>(this->gameGlobal, startButtonRectangle, "Start");
}

/**
 * @function handleEvents
 *
 * Handle SDL events that occur in the main menu state.
 * @param gameIsRunning - Whether or not the game is running.
 * @output - Current state the game is in.
 */
int MainMenu::handleEvents(bool* gameIsRunning) {
  SDL_Event event;
  int returnValue = 0;
  while (SDL_PollEvent(&event) != 0) { // While there are events in the queue
    switch (event.type) {              // Check which type of event
    case SDL_QUIT:
      *gameIsRunning = false;
      break;

    case SDL_MOUSEBUTTONDOWN:
      if (this->startButton->checkHovered(event.motion.x, event.motion.y) == 0) {
        break; // Stay in main menu state
      }
      //      writeToLogFile(this->gameGlobal.logFile,
      //                    "Main menu button was pressed, switching to gameplay state");
      returnValue = 1; // Mouse was over button, switch to gameplay state
      break;

    default:
      break;
    }
  }
  return returnValue;
}

/**
 * @function render
 *
 * Render the game title and the start button
 * @param none
 * @output none
 */
void MainMenu::render() {
  SDL_SetRenderDrawColor(this->gameGlobal.renderer, 0, 0, 0, 255); // Black background
  SDL_RenderClear(this->gameGlobal.renderer);
  this->startButton->render();
  this->title->render();
  SDL_RenderPresent(this->gameGlobal.renderer);
}
