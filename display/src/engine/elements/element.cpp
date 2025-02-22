#include <SDL2/SDL.h>
#include <iostream>

#include "element.h"

/**
 * Template specialization for centering vertically within an SDL_Rect.
 *
 * @param centerWithin The SDL_Rect to center within
 * @return None
 */
template <> void Element::centerVertical<SDL_Rect>(SDL_Rect centerWithin) {
  this->boundaryRectangle.y =
      (centerWithin.h / 2 - this->boundaryRectangle.h / 2) + centerWithin.y;
}

/**
 * Template specialization for centering horizontally within an SDL_Rect.
 *
 * @param centerWithin The SDL_Rect to center within
 * @return None
 */
template <> void Element::centerHorizontal<SDL_Rect>(SDL_Rect centerWithin) {
  this->boundaryRectangle.x =
      (centerWithin.w / 2 - this->boundaryRectangle.w / 2) + centerWithin.x;
}

/**
 * Template specialization for checking if centered vertically within an SDL_Rect.
 *
 * @param centerWithin The SDL_Rect to center within
 * @return None
 */
template <> bool Element::checkCenterVertical<SDL_Rect>(SDL_Rect centerWithin) {
  bool centered = false;
  if (this->boundaryRectangle.y ==
      (centerWithin.h / 2 - this->boundaryRectangle.h / 2) + centerWithin.y) {
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
  if (this->boundaryRectangle.x ==
      (centerWithin.w / 2 - this->boundaryRectangle.w / 2) + centerWithin.x) {
    centered = true;
  }
  return centered;
}

/*
Element::Element()
    : boundaryRectangle{0, 0, 0, 0}, parent(nullptr), positionRelativeToParent{0, 0} {}
    */

void Element::setboundaryRectangle(SDL_Rect boundaryRectangle) {
  this->boundaryRectangle = boundaryRectangle;
};

void Element::setPositionRelativeToParent(const SDL_Point& relativePostion) {
  this->positionRelativeToParent = relativePostion;
}

void Element::setParent(Element* parent) { this->parent = parent; }

void Element::update() {
  if (parent) {
    boundaryRectangle.x = parent->boundaryRectangle.x + positionRelativeToParent.x;
    boundaryRectangle.y = parent->boundaryRectangle.y + positionRelativeToParent.y;
  }
}

/**
 * Add a border to an element.
 *
 * @param borderThickness How many pixels thick the border should be
 * @return None
 */
void Element::addBorder(const int& borderThickness) {
  this->hasBorder       = true;
  this->borderThickness = borderThickness;
}

/**
 * Render a border around an object. Draws 4 lines on each side of the element's
 * boundaryRectangle.
 *
 * @param None
 * @return None
 */
void Element::renderBorder() const {
  SDL_SetRenderDrawColor(this->displayGlobal.renderer, 255, 255, 255, 255);
  // Draw the top border
  for (int i = 0; i < borderThickness; i++) {
    SDL_RenderDrawLine(this->displayGlobal.renderer,
                       this->boundaryRectangle.x - i, // Start X
                       this->boundaryRectangle.y - i, // Start Y
                       this->boundaryRectangle.x + this->boundaryRectangle.w + i, // End X
                       this->boundaryRectangle.y - i);                            // End Y
  }

  // Draw the bottom border
  for (int i = 0; i < borderThickness; i++) {
    SDL_RenderDrawLine(
        this->displayGlobal.renderer,
        this->boundaryRectangle.x - i,                              // Start X
        this->boundaryRectangle.y + this->boundaryRectangle.h + i,  // Start Y
        this->boundaryRectangle.x + this->boundaryRectangle.w + i,  // End X
        this->boundaryRectangle.y + this->boundaryRectangle.h + i); // End Y
  }

  // Draw the left border
  for (int i = 0; i < borderThickness; i++) {
    SDL_RenderDrawLine(this->displayGlobal.renderer,
                       this->boundaryRectangle.x - i, // Start X
                       this->boundaryRectangle.y - i, // Start Y
                       this->boundaryRectangle.x - i, // End X
                       this->boundaryRectangle.y + this->boundaryRectangle.h +
                           i); // End Y
  }

  // Draw the right border
  for (int i = 0; i < borderThickness; i++) {
    SDL_RenderDrawLine(
        this->displayGlobal.renderer,
        this->boundaryRectangle.x + this->boundaryRectangle.w + i,  // Start X
        this->boundaryRectangle.y - i,                              // Start Y
        this->boundaryRectangle.x + this->boundaryRectangle.w + i,  // End X
        this->boundaryRectangle.y + this->boundaryRectangle.h + i); // End Y
  }
}
