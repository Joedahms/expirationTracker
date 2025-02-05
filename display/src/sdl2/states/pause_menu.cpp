#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>

#include "../display_global.h"
#include "pause_menu.h"

/**
 * Input:
 * - displayGlobal - Global display variables
 * Output: None
 */
PauseMenu::PauseMenu(struct DisplayGlobal displayGlobal) {
  this->displayGlobal        = displayGlobal;
  SDL_Surface* windowSurface = SDL_GetWindowSurface(this->displayGlobal.window);

  // Title text
  const char* titleContent = "Paused";
  SDL_Color titleColor     = {0, 255, 0, 255}; // Green
  SDL_Rect titleRectangle  = {100, 100, 0, 0};
  std::unique_ptr<Text> title =
      std::make_unique<Text>(this->displayGlobal, displayGlobal.futuramFontPath,
                             titleContent, 24, titleColor, titleRectangle);
  title->centerHorizontal(windowSurface);
  this->texts.push_back(std::move(title));

  // Resume button
  SDL_Rect resumeButtonRectangle = {200, 150, 200, 50};
  std::unique_ptr<Button> resumeButton =
      std::make_unique<Button>(this->displayGlobal, resumeButtonRectangle, "Resume");
  resumeButton->centerHorizontal(windowSurface);
  this->buttons.push_back(std::move(resumeButton));

  // Main menu button
  SDL_Rect mainMenuButtonRectangle = {200, 225, 200, 50};
  std::unique_ptr<Button> mainMenuButton =
      std::make_unique<Button>(this->displayGlobal, mainMenuButtonRectangle, "Main Menu");
  mainMenuButton->centerHorizontal(windowSurface);
  this->buttons.push_back(std::move(resumeButton));
}

/**
 * Handle SDL events that occur in the pause menu state.
 *
 * Input:
 * - displayIsRunning - Whether or not the display is running.
 * Output: Current state the display is in.
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
 * Render paused and the button
 *
 * Input: None
 * Output: None
 */
void PauseMenu::render() const {
  SDL_SetRenderDrawColor(this->displayGlobal.renderer, 0, 0, 0, 255); // Black background
  SDL_RenderClear(this->displayGlobal.renderer);
  renderElements();
  SDL_RenderPresent(this->displayGlobal.renderer);
}
