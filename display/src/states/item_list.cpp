#include <SDL2/SDL_ttf.h>
#include <memory>

#include "item_list.h"

/**
 * @param displayGlobal Global display variables.
 * @return None
 */
ItemList::ItemList(struct DisplayGlobal displayGlobal) {
  this->displayGlobal        = displayGlobal;
  SDL_Surface* windowSurface = SDL_GetWindowSurface(this->displayGlobal.window);

  // Placeholder text
  const char* fontPath               = "../display/fonts/16020_FUTURAM.ttf";
  const char* placeholderTextContent = "Placeholder for item list";
  SDL_Color placeholderTextColor     = {0, 255, 0, 255}; // Green
  SDL_Rect placeholderTextRectangle  = {
      100,
      100,
      0,
      0,
  }; // x y w h
  this->placeholderText =
      std::make_unique<Text>(this->displayGlobal, fontPath, placeholderTextContent, 24,
                             placeholderTextColor, placeholderTextRectangle);
  this->placeholderText->centerHorizontal(windowSurface);
}

int ItemList::handleEvents(bool* displayIsRunning) {
  SDL_Event event;
  while (SDL_PollEvent(&event) != 0) { // While events in the queue
    switch (event.type) {
    case SDL_QUIT: // Quit event
      *displayIsRunning = false;
      break;
      // Touch event here
    default:
      break;
    }
  }

  return ITEM_LIST;
}

/**
 * Perform the appropriate action depending on which keyboard key has been pressed.
 *
 * @param None
 * @return The state the display is in after checking if any keys have been pressed
 */
int ItemList::checkKeystates() {
  const Uint8* keystates = SDL_GetKeyboardState(NULL);

  if (keystates[SDL_SCANCODE_ESCAPE]) {
    return PAUSE_MENU;
  }

  return ITEM_LIST;
}

void ItemList::render() {
  SDL_SetRenderDrawColor(this->displayGlobal.renderer, 0, 0, 0, 255); // Black background
  SDL_RenderClear(this->displayGlobal.renderer);
  this->placeholderText->render();
  SDL_RenderPresent(this->displayGlobal.renderer);
}
