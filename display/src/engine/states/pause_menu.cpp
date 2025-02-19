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
  this->displayGlobal        = displayGlobal;
  SDL_Surface* windowSurface = SDL_GetWindowSurface(this->displayGlobal.window);

  this->rootElement = std::make_unique<CompositeElement>();

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
  SDL_Rect resumeButtonRectangle       = {200, 150, 200, 50};
  std::unique_ptr<Button> resumeButton = std::make_unique<Button>(
      this->displayGlobal, resumeButtonRectangle, "Resume", SCANNING);
  resumeButton->centerHorizontal(windowSurface);
  this->rootElement->addElement(std::move(resumeButton));

  // Main menu button
  SDL_Rect mainMenuButtonRectangle       = {200, 225, 200, 50};
  std::unique_ptr<Button> mainMenuButton = std::make_unique<Button>(
      this->displayGlobal, mainMenuButtonRectangle, "Main Menu", MAIN_MENU);
  mainMenuButton->centerHorizontal(windowSurface);
  this->rootElement->addElement(std::move(mainMenuButton));
}

/**
 * Handle SDL events that occur in the pause menu state.
 *
 * @param displayIsRunning Whether or not the display is running.
 * @return Current state the display is in.
 */
int PauseMenu::handleEvents(bool* displayIsRunning) {
  SDL_Event event;
  int returnValue = 2;
  while (SDL_PollEvent(&event) != 0) { // While there are events in the queue
    int mouseX = event.motion.x;
    int mouseY = event.motion.y;
    switch (event.type) { // Check which type of event
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
