#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>

#include "../display_global.h"
#include "../elements/composite_element.h"
#include "main_menu.h"
#include "state.h"

/**
 * @param displayGlobal Global display variables.
 */
MainMenu::MainMenu(struct DisplayGlobal displayGlobal) {
  this->displayGlobal        = displayGlobal;
  SDL_Surface* windowSurface = SDL_GetWindowSurface(this->displayGlobal.window);

  this->rootElement = std::make_unique<CompositeElement>();

  const char* titleContent = "Expiration Tracker";
  SDL_Color titleColor     = {0, 255, 0, 255}; // Green
  SDL_Rect titleRectangle  = {100, 100, 0, 0};
  std::unique_ptr<Text> title =
      std::make_unique<Text>(this->displayGlobal, this->displayGlobal.futuramFontPath,
                             titleContent, 24, titleColor, titleRectangle);
  title->centerHorizontal(windowSurface);
  rootElement->addElement(std::move(title));

  SDL_Rect newScanButtonRectangle       = {200, 150, 200, 50};
  std::unique_ptr<Button> newScanButton = std::make_unique<Button>(
      this->displayGlobal, newScanButtonRectangle, "Scan New Item", SCANNING);
  newScanButton->centerHorizontal(windowSurface);
  rootElement->addElement(std::move(newScanButton));

  SDL_Rect viewStoredButtonRectangle       = {200, 210, 200, 50};
  std::unique_ptr<Button> viewStoredButton = std::make_unique<Button>(
      this->displayGlobal, viewStoredButtonRectangle, "View Stored Items", ITEM_LIST);
  viewStoredButton->centerHorizontal(windowSurface);
  rootElement->addElement(std::move(viewStoredButton));
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

    switch (event.type) {
    case SDL_QUIT:
      *displayIsRunning = false;
      break;

    case SDL_MOUSEBUTTONDOWN:
      returnValue = checkButtonsClicked(mouseX, mouseY);
      break;

    default:
      break;
    }
  }
  return returnValue;
}

/**
 * Perform operations that need to be done periodically within the state. Update all
 * buttons.
 *
 * @param None
 * @return None
 */
void MainMenu::update() { this->rootElement->update(); }

/**
 * Render all elements.
 *
 * @param None
 * @return None
 */
void MainMenu::render() const {
  SDL_SetRenderDrawColor(this->displayGlobal.renderer, 0, 0, 0, 255); // Black background
  SDL_RenderClear(this->displayGlobal.renderer);
  this->rootElement->render();
  SDL_RenderPresent(this->displayGlobal.renderer);
}
