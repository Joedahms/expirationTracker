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
  const char* fontPath     = "../display/fonts/16020_FUTURAM.ttf";
  const char* titleContent = "Expiration Tracker";
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

  // Scan New Item
  SDL_Rect newScanButtonRectangle = {200, 150, 200, 50};
  newScanButtonRectangle =
      centerRectangleHorizontal(windowSurface, newScanButtonRectangle);
  this->newScanButton = std::make_unique<Button>(this->displayGlobal,
                                                 newScanButtonRectangle, "Scan New Item");

  // View Stored Items
  SDL_Rect viewStoredButtonRectangle = {200, 210, 200, 50};
  viewStoredButtonRectangle =
      centerRectangleHorizontal(windowSurface, viewStoredButtonRectangle);
  this->viewStoredButton = std::make_unique<Button>(
      this->displayGlobal, viewStoredButtonRectangle, "View Stored Items");
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
      if (this->newScanButton->checkHovered(event.motion.x, event.motion.y) == 0) {
        break; // Stay in main menu state
      }
      returnValue = 1; // Mouse was over button, switch to gameplay state
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
  this->newScanButton->render();
  this->viewStoredButton->render();
  this->title->render();
  SDL_RenderPresent(this->displayGlobal.renderer);
}
