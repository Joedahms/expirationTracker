#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>

#include "../display_global.h"
#include "../rectangle.h"
#include "main_menu.h"

/**
 * @param displayGlobal Global display variables.
 * @return None
 */
MainMenu::MainMenu(struct DisplayGlobal displayGlobal) {
  this->displayGlobal        = displayGlobal;
  SDL_Surface* windowSurface = SDL_GetWindowSurface(this->displayGlobal.window);

  // Title
  const char* fontPath     = "../display/fonts/16020_FUTURAM.ttf";
  const char* titleContent = "Expiration Tracker";
  SDL_Color titleColor     = {0, 255, 0, 255}; // Green
  SDL_Rect titleRectangle  = {
      100,
      100,
      0,
      0,
  }; // x y w h
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
 * @param displayIsRunning Whether or not the display is running.
 * @return Current state the display is in.
 */
int MainMenu::handleEvents(bool* displayIsRunning) {
  SDL_Event event;
  int returnValue = MAIN_MENU;
  while (SDL_PollEvent(&event) != 0) { // While there are events in the queue
    int mouseX = event.motion.x;
    int mouseY = event.motion.y;

    switch (event.type) { // Check which type of event
    case SDL_QUIT:
      *displayIsRunning = false;
      break;

    // Check if mouse was clicked over any buttons
    case SDL_MOUSEBUTTONDOWN:
      if (this->newScanButton->checkHovered(mouseX, mouseY) == true) {
        returnValue = SCANNING;
        break;
      }
      else if (this->viewStoredButton->checkHovered(mouseX, mouseY) == true) {
        returnValue = ITEM_LIST;
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
 * Render the display title and the start button.
 *
 * @param None
 * @return None
 */
void MainMenu::render() {
  SDL_SetRenderDrawColor(this->displayGlobal.renderer, 0, 0, 0, 255); // Black background
  SDL_RenderClear(this->displayGlobal.renderer);
  this->newScanButton->render();
  this->viewStoredButton->render();
  this->title->render();
  SDL_RenderPresent(this->displayGlobal.renderer);
}
