#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>

#include "../display_global.h"
#include "pause_menu.h"

/**
 * Input:
 * @param displayGlobal Global display variables
 * @return None
 */
PauseMenu::PauseMenu(struct DisplayGlobal displayGlobal) {
  this->currentState         = EngineState::PAUSE_MENU;
  this->displayGlobal        = displayGlobal;
  SDL_Surface* windowSurface = SDL_GetWindowSurface(this->displayGlobal.window);

  this->rootElement = std::make_unique<Container>();

  // Title text
  const char* titleContent = "Paused";
  SDL_Color titleColor     = {0, 255, 0, 255}; // Green
  SDL_Rect titleRectangle  = {100, 100, 0, 0};
  std::unique_ptr<Text> title =
      std::make_unique<Text>(this->displayGlobal, displayGlobal.futuramFontPath,
                             titleContent, 24, titleColor, titleRectangle);
  title->centerHorizontal(windowSurface);
  this->rootElement->addElement(std::move(title));

  // Resume button
  SDL_Rect resumeButtonRectangle = {200, 150, 200, 50};
  std::unique_ptr<Button> resumeButton =
      std::make_unique<Button>(this->displayGlobal, resumeButtonRectangle, "Resume",
                               [this]() { this->currentState = EngineState::SCANNING; });
  resumeButton->centerHorizontal(windowSurface);
  this->rootElement->addElement(std::move(resumeButton));

  // Main menu button
  SDL_Rect mainMenuButtonRectangle = {200, 225, 200, 50};
  std::unique_ptr<Button> mainMenuButton =
      std::make_unique<Button>(this->displayGlobal, mainMenuButtonRectangle, "Main Menu",
                               [this]() { this->currentState = EngineState::MAIN_MENU; });
  mainMenuButton->centerHorizontal(windowSurface);
  this->rootElement->addElement(std::move(mainMenuButton));
}

/**
 * Perform operations that need to be done periodically within the state. Update all
 * buttons.
 *
 * @param None
 * @return None
 */
void PauseMenu::update() { this->rootElement->update(); }

/**
 * Render all elements.
 *
 * @param None
 * @return None
 */
void PauseMenu::render() const {
  SDL_SetRenderDrawColor(this->displayGlobal.renderer, 0, 0, 0, 255); // Black background
  SDL_RenderClear(this->displayGlobal.renderer);
  this->rootElement->render();
  SDL_RenderPresent(this->displayGlobal.renderer);
}
