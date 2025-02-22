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
  this->currentState = EngineState::MAIN_MENU;

  this->displayGlobal        = displayGlobal;
  SDL_Surface* windowSurface = SDL_GetWindowSurface(this->displayGlobal.window);

  this->rootElement = std::make_unique<Container>();

  // Title
  const char* titleContent = "Expiration Tracker";
  SDL_Color titleColor     = {0, 255, 0, 255}; // Green
  SDL_Rect titleRectangle  = {100, 100, 0, 0};
  std::unique_ptr<Text> title =
      std::make_unique<Text>(this->displayGlobal, this->displayGlobal.futuramFontPath,
                             titleContent, 24, titleColor, titleRectangle);
  title->centerHorizontal(windowSurface);
  rootElement->addElement(std::move(title));

  // Start Scan
  SDL_Rect newScanButtonRectangle       = {200, 150, 200, 50};
  std::unique_ptr<Button> newScanButton = std::make_unique<Button>(
      this->displayGlobal, newScanButtonRectangle, "Scan New Item",
      [this]() { this->currentState = EngineState::SCANNING; });
  newScanButton->centerHorizontal(windowSurface);
  rootElement->addElement(std::move(newScanButton));

  // View Stored
  SDL_Rect viewStoredButtonRectangle       = {200, 210, 200, 50};
  std::unique_ptr<Button> viewStoredButton = std::make_unique<Button>(
      this->displayGlobal, viewStoredButtonRectangle, "View Stored Items",
      [this]() { this->currentState = EngineState::ITEM_LIST; });
  viewStoredButton->centerHorizontal(windowSurface);
  rootElement->addElement(std::move(viewStoredButton));
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
