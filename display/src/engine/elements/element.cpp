#include <SDL2/SDL.h>
#include <iostream>

#include "element.h"

void Element::setboundaryRectangle(SDL_Rect boundaryRectangle) {
  this->boundaryRectangle = boundaryRectangle;
};

void Element::setPositionRelativeToParent(const SDL_Point& relativePostion) {
  this->positionRelativeToParent = relativePostion;
}

void Element::setParent(Element* parent) { this->parent = parent; }

void Element::setCentered() { this->centerWithinParent = true; }
void Element::setCenteredVertical() { this->centerVerticalWithinParent = true; }
void Element::setCenteredHorizontal() { this->centerHorizontalWithinParent = true; }

bool Element::checkCenterVertical() {
  bool centered = false;
  if (this->positionRelativeToParent.y ==
      (this->parent->boundaryRectangle.h / 2 - this->boundaryRectangle.h / 2) +
          this->parent->boundaryRectangle.y) {
    centered = true;
  }
  return centered;
}

void Element::centerVertical() {
  this->positionRelativeToParent.y =
      (this->parent->boundaryRectangle.h / 2 - this->boundaryRectangle.h / 2) +
      this->parent->boundaryRectangle.y;
}

bool Element::checkCenterHorizontal() {
  bool centered = false;
  if (this->positionRelativeToParent.x ==
      (this->parent->boundaryRectangle.w / 2 - this->boundaryRectangle.w / 2) +
          this->parent->boundaryRectangle.x) {
    centered = true;
  }
  return centered;
}

void Element::centerHorizontal() {
  this->positionRelativeToParent.x =
      ((this->parent->boundaryRectangle.w - this->boundaryRectangle.w) / 2) +
      this->parent->boundaryRectangle.x;
  /*
  this->positionRelativeToParent.x =
      (this->parent->boundaryRectangle.w / 2 - this->boundaryRectangle.w / 2) +
      this->parent->boundaryRectangle.x;
      */
}

void Element::update() {
  if (parent) {
    if (this->centerWithinParent) {
      if (checkCenterVertical() == false) {
        centerVertical();
      }
      if (checkCenterHorizontal() == false) {
        centerHorizontal();
      }
    }
    else if (this->centerVerticalWithinParent) {
      if (checkCenterVertical() == false) {
        centerVertical();
      }
    }
    else if (this->centerHorizontalWithinParent) {
      if (checkCenterHorizontal() == false) {
        centerHorizontal();
      }
    }

    this->boundaryRectangle.x =
        this->parent->boundaryRectangle.x + this->positionRelativeToParent.x;
    this->boundaryRectangle.y =
        this->parent->boundaryRectangle.y + this->positionRelativeToParent.y;
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
