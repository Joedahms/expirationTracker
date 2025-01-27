#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>

#include "display_global.h"
#include "main_menu.h"
#include "rectangle.h"

/**
 * Input:
 * - displayGlobal - Global display variables
 * Output: None
 */
MainMenu::MainMenu(struct DisplayGlobal displayGlobal) {
  this->displayGlobal        = displayGlobal;
  SDL_Surface* windowSurface = SDL_GetWindowSurface(this->displayGlobal.window);

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
  this->title = std::make_unique<Text>(this->displayGlobal, fontPath, titleContent, 24,
                                       titleColor, titleRectangle);
  this->title->centerHorizontal(windowSurface);

  // Start button
  SDL_Rect startButtonRectangle = {200, 150, 200, 50};
  startButtonRectangle = centerRectangleHorizontal(windowSurface, startButtonRectangle);
  this->startButton =
      std::make_unique<Button>(this->displayGlobal, startButtonRectangle, "Start");
}

/**
 * Handle SDL events that occur in the main menu state.
 *
 * Input:
 * - displayIsRunning - Whether or not the display is running.
 * Output: Current state the display is in.
 */
int MainMenu::handleEvents(bool* displayIsRunning) {
  SDL_Event event;
  int returnValue = 0;
  while (SDL_PollEvent(&event) != 0) { // While there are events in the queue
    switch (event.type) {              // Check which type of event
    case SDL_QUIT:
      *displayIsRunning = false;
      break;

    case SDL_MOUSEBUTTONDOWN:
      if (this->startButton->checkHovered(event.motion.x, event.motion.y) == 0) {
        break; // Stay in main menu state
      }
      //      writeToLogFile(this->displayGlobal.logFile,
      //                    "Main menu button was pressed, switching to displayplay
      //                    state");
      returnValue = 1; // Mouse was over button, switch to displayplay state
      break;

    default:
      break;
    }
  }
  return returnValue;
}

/**
 * Render the display title and the start button
 *
 * Input: None
 * Output: None
 */
void MainMenu::render() {
  SDL_SetRenderDrawColor(this->displayGlobal.renderer, 0, 0, 0, 255); // Black background
  SDL_RenderClear(this->displayGlobal.renderer);
  this->startButton->render();
  this->title->render();
  SDL_RenderPresent(this->displayGlobal.renderer);
}
