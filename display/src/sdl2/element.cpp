#include <SDL2/SDL.h>

#include "element.h"

/**
 * Template specialization for centering vertically within an SDL_Rect.
 *
 * @param centerWithin The SDL_Rect to center within
 * @return None
 */
template <> void Element::centerVertical<SDL_Rect>(SDL_Rect centerWithin) {
  this->rectangle.y = (centerWithin.h / 2 - this->rectangle.h / 2) + centerWithin.y;
}

/**
 * Template specialization for centering horizontally within an SDL_Rect.
 *
 * @param centerWithin The SDL_Rect to center within
 * @return None
 */
template <> void Element::centerHorizontal<SDL_Rect>(SDL_Rect centerWithin) {
  this->rectangle.x = (centerWithin.w / 2 - this->rectangle.w / 2) + centerWithin.x;
}

/**
 * Template specialization for checking if centered vertically within an SDL_Rect.
 *
 * @param centerWithin The SDL_Rect to center within
 * @return None
 */
template <> bool Element::checkCenterVertical<SDL_Rect>(SDL_Rect centerWithin) {
  bool centered = false;
  if (this->rectangle.y ==
      (centerWithin.h / 2 - this->rectangle.h / 2) + centerWithin.y) {
    centered = true;
  }
  return centered;
}

/**
 * Template specialization for checking if centered horizontally within an SDL_Rect.
 *
 * @param centerWithin The SDL_Rect to center within
 * @return None
 */
template <> bool Element::checkCenterHorizontal<SDL_Rect>(SDL_Rect centerWithin) {
  bool centered = false;
  if (this->rectangle.x ==
      (centerWithin.w / 2 - this->rectangle.w / 2) + centerWithin.x) {
    centered = true;
  }
  return centered;
}

void Element::setRectangle(SDL_Rect rectangle) { this->rectangle = rectangle; };

/*
void Element::renderBorder() {
  SDL_SetRenderDrawColor(this->displayGlobal.renderer, 255, 255, 255, 255);
  // Draw the top border
  for (int i = 0; i < borderThickness; i++) {
    SDL_RenderDrawLine(renderer,
                       rect.x - i,          // Start X
                       rect.y - i,          // Start Y
                       rect.x + rect.w + i, // End X
                       rect.y - i);         // End Y
  }

  // Draw the bottom border
  for (int i = 0; i < borderThickness; i++) {
    SDL_RenderDrawLine(renderer,
                       rect.x - i,           // Start X
                       rect.y + rect.h + i,  // Start Y
                       rect.x + rect.w + i,  // End X
                       rect.y + rect.h + i); // End Y
  }

  // Draw the left border
  for (int i = 0; i < borderThickness; i++) {
    SDL_RenderDrawLine(renderer,
                       rect.x - i,           // Start X
                       rect.y - i,           // Start Y
                       rect.x - i,           // End X
                       rect.y + rect.h + i); // End Y
  }

  // Draw the right border
  for (int i = 0; i < borderThickness; i++) {
    SDL_RenderDrawLine(renderer,
                       rect.x + rect.w + i,  // Start X
                       rect.y - i,           // Start Y
                       rect.x + rect.w + i,  // End X
                       rect.y + rect.h + i); // End Y
  }
}
*/
