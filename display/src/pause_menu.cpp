#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>

#include "game_global.h"
#include "pause_menu.h"
#include "rectangle.h"

/**
 * @function: PauseMenu
 *
 * PauseMenu constructor.
 * @param gameGlobal - Global game variables
 * @output - None
 */
PauseMenu::PauseMenu(struct GameGlobal gameGlobal) {
  this->gameGlobal           = gameGlobal;
  SDL_Surface* windowSurface = SDL_GetWindowSurface(this->gameGlobal.window);

  // Title
  const char* fontPath     = "../16020_FUTURAM.ttf";
  const char* titleContent = "Paused";
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

  // Resume button
  SDL_Rect resumeButtonRectangle = {200, 150, 200, 50};
  resumeButtonRectangle = centerRectangleHorizontal(windowSurface, resumeButtonRectangle);
  this->resumeButton =
      std::make_unique<Button>(this->gameGlobal, resumeButtonRectangle, "Resume");

  // Main menu button
  SDL_Rect mainMenuButtonRectangle = {200, 225, 200, 50};
  mainMenuButtonRectangle =
      centerRectangleHorizontal(windowSurface, mainMenuButtonRectangle);
  this->mainMenuButton =
      std::make_unique<Button>(this->gameGlobal, mainMenuButtonRectangle, "Main Menu");
}

/**
 * @function handleEvents
 *
 * Handle SDL events that occur in the pause menu state.
 * @param gameIsRunning - Whether or not the game is running.
 * @output - Current state the game is in.
 */
int PauseMenu::handleEvents(bool* gameIsRunning) {
  SDL_Event event;
  int returnValue = 2;
  while (SDL_PollEvent(&event) != 0) { // While there are events in the queue
    switch (event.type) {              // Check which type of event
    case SDL_QUIT:
      *gameIsRunning = false;
      break;

    case SDL_MOUSEBUTTONDOWN:
      if (this->resumeButton->checkHovered(event.motion.x, event.motion.y) == 0) {
        ;
      }
      else {
        //        writeToLogFile(this->gameGlobal.logFile,
        //                      "Resume was pressed, switching from pause menu to
        //                      gameplay");
        returnValue = 1; // Mouse was over button, switch to gameplay state
        break;
      }

      if (this->mainMenuButton->checkHovered(event.motion.x, event.motion.y) == 0) {
        ;
      }
      else {
        //      writeToLogFile(this->gameGlobal.logFile,
        //                     "Main menu was pressed, switching from pause menu to main
        //                     menu");
        returnValue = 0; // Mouse was over button, switch to main menu state
        break;
      }

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
 * Render paused and the button
 * @param - None
 * @output - None
 */
void PauseMenu::render() {
  SDL_SetRenderDrawColor(this->gameGlobal.renderer, 0, 0, 0, 255); // Black background
  SDL_RenderClear(this->gameGlobal.renderer);
  this->resumeButton->render();
  this->mainMenuButton->render();
  this->title->render();
  SDL_RenderPresent(this->gameGlobal.renderer);
}
