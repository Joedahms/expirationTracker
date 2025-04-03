#include <SDL2/SDL.h>
#include <iostream>

#include "element.h"

void Element::render() const {
  if (this->hasBorder) {
    renderBorder();
  }
}

/**
 * Ensure that the positioning of the element is correct. Make sure that if need be, it is
 * centered with respect to its parent (if it has one) and that it is in the correct
 * position relative to its parent (if it has one).
 *
 * @param None
 * @return None
 */
void Element::update() {
  if (parent) {
    hasParentUpdate();
  }
}

/**
 * Updates to perform if the element has a parent. Checks centering within parent and
 * ensures the position of the element is correct giving its current position relative to
 * parent.
 *
 * @param None
 * @return None
 */
void Element::hasParentUpdate() {
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

  if (this->held) {
    this->positionRelativeToParent.x += this->velocity.x;
    this->positionRelativeToParent.y += this->velocity.y;
    updatePosition();

    this->velocity.x = 0;
    this->velocity.y = 0;
  }
  else {
    if (!this->fixed) {
      this->acceleration.x = 0;
      this->acceleration.y = 0.1;
    }

    this->velocity.x += this->acceleration.x;
    this->velocity.y += this->acceleration.y;

    this->positionRelativeToParent.x += this->velocity.x;
    this->positionRelativeToParent.y += this->velocity.y;
    updatePosition();
  }
}

void Element::updatePosition() {
  SDL_Rect parentBoundaryRectangle = parent->getBoundaryRectangle();
  this->boundaryRectangle.x =
      parentBoundaryRectangle.x + this->positionRelativeToParent.x;
  this->boundaryRectangle.y =
      parentBoundaryRectangle.y + this->positionRelativeToParent.y;
}

std::string Element::getContent() const { return "no content"; }

void Element::setMediator(std::shared_ptr<Mediator> mediator) {
  this->mediator = mediator;
}
void Element::setParent(Element* parent) { this->parent = parent; }

void Element::setPositionRelativeToParent(const SDL_Point& relativePostion) {
  this->positionRelativeToParent = relativePostion;
}
SDL_Point Element::getPositionRelativeToParent() {
  return this->positionRelativeToParent;
}

SDL_Rect Element::getBoundaryRectangle() { return this->boundaryRectangle; }
void Element::setBoundaryRectangle(SDL_Rect boundaryRectangle) {
  this->boundaryRectangle = boundaryRectangle;
};

void Element::setCentered() { this->centerWithinParent = true; }

/**
 * Centering vertically means centering on the y axis.
 */
void Element::setCenteredVertical() { this->centerVerticalWithinParent = true; }
bool Element::checkCenterVertical() {
  bool centered = false;
  if (this->positionRelativeToParent.y ==
      (this->parent->boundaryRectangle.h / 2 - this->boundaryRectangle.h / 2)) {
    centered = true;
  }
  return centered;
}
void Element::centerVertical() {
  this->positionRelativeToParent.y =
      (this->parent->boundaryRectangle.h / 2 - this->boundaryRectangle.h / 2);
}

/**
 * Centering horizontally means centering on the x axis.
 */
void Element::setCenteredHorizontal() { this->centerHorizontalWithinParent = true; }
bool Element::checkCenterHorizontal() {
  bool centered = false;
  if (this->positionRelativeToParent.x ==
      (this->parent->boundaryRectangle.w / 2 - this->boundaryRectangle.w / 2)) {
    centered = true;
  }
  return centered;
}
void Element::centerHorizontal() {
  this->positionRelativeToParent.x =
      ((this->parent->boundaryRectangle.w - this->boundaryRectangle.w) / 2);
}

/**
 * Check if the mouse is over the element.
 *
 * @param None
 * @return Whether or not the mouse is over the button
 */
bool Element::checkMouseHovered() {
  SDL_Point mousePosition;
  SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

  if (SDL_PointInRect(&mousePosition, &this->boundaryRectangle)) {
    return true;
  }
  return false;
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
                       this->positionRelativeToParent.x - i, // Start X
                       this->positionRelativeToParent.y - i, // Start Y
                       this->positionRelativeToParent.x + this->boundaryRectangle.w +
                           i,                                 // End X
                       this->positionRelativeToParent.y - i); // End Y
  }

  // Draw the bottom border
  for (int i = 0; i < borderThickness; i++) {
    SDL_RenderDrawLine(
        this->displayGlobal.renderer,
        this->positionRelativeToParent.x - i,                              // Start X
        this->positionRelativeToParent.y + this->boundaryRectangle.h + i,  // Start Y
        this->positionRelativeToParent.x + this->boundaryRectangle.w + i,  // End X
        this->positionRelativeToParent.y + this->boundaryRectangle.h + i); // End Y
  }

  // Draw the left border
  for (int i = 0; i < borderThickness; i++) {
    SDL_RenderDrawLine(this->displayGlobal.renderer,
                       this->positionRelativeToParent.x - i, // Start X
                       this->positionRelativeToParent.y - i, // Start Y
                       this->positionRelativeToParent.x - i, // End X
                       this->positionRelativeToParent.y + this->boundaryRectangle.h +
                           i); // End Y
  }

  // Draw the right border
  for (int i = 0; i < borderThickness; i++) {
    SDL_RenderDrawLine(
        this->displayGlobal.renderer,
        this->positionRelativeToParent.x + this->boundaryRectangle.w + i,  // Start X
        this->positionRelativeToParent.y - i,                              // Start Y
        this->positionRelativeToParent.x + this->boundaryRectangle.w + i,  // End X
        this->positionRelativeToParent.y + this->boundaryRectangle.h + i); // End Y
  }
}

void Element::setupPosition(const SDL_Rect& boundaryRectangle) {
  this->boundaryRectangle          = boundaryRectangle;
  this->positionRelativeToParent.x = boundaryRectangle.x;
  this->positionRelativeToParent.y = boundaryRectangle.y;

  if (parent) {
    SDL_Rect parentRectangle  = this->parent->getBoundaryRectangle();
    this->boundaryRectangle.x = parentRectangle.x + this->positionRelativeToParent.x;
    this->boundaryRectangle.y = parentRectangle.y + this->positionRelativeToParent.y;
  }
}

Velocity Element::getVelocity() { return this->velocity; }
void Element::setVelocity(Velocity velocity) { this->velocity = velocity; }
Acceleration Element::getAcceleration() { return this->acceleration; }
void Element::setAcceleration(Acceleration acceleration) {
  this->acceleration = acceleration;
}

int Element::getBorderThickness() { return this->borderThickness; }
bool Element::getFixed() { return this->fixed; }
bool Element::getScreenBoundX() { return this->screenBoundX; }
bool Element::getScreenBoundY() { return this->screenBoundY; }

void Element::addBoundaryRectangle(std::vector<SDL_Rect>& boundaryRectangles) const {
  if (this->canCollide) {
    boundaryRectangles.push_back(this->boundaryRectangle);
  }
}

/**
 * If the element's position is not fixed in the event of a collision, check if it
 * has collided with any other element that can be collided with.
 *
 * @param boundaryRectangles Boundary rectangles for all objects that can be collided
 * with.
 * @return None
 */
void Element::checkCollision(std::vector<SDL_Rect>& boundaryRectangles) {
  if (!this->canCollide || this->collisionFixed) {
    return;
  }
  checkCollisionImpl(boundaryRectangles);
}

/**
 * Implementation of collision checking. Check against all other objects that can be
 * collided with. If there is a collision, adjust position so that there is no collision.
 *
 * @param boundaryRectangles Boundary rectangles for all objects that can be collided
 * with.
 * @return None
 */
void Element::checkCollisionImpl(std::vector<SDL_Rect>& boundaryRectangles) {
  for (auto& boundaryRectangle : boundaryRectangles) {
    if (SDL_RectEquals(&this->boundaryRectangle, &boundaryRectangle)) {
      continue;
    }

    if (!SDL_HasIntersection(&this->boundaryRectangle, &boundaryRectangle)) {
      continue;
    }

    SDL_Point overlap = calculateOverlap(boundaryRectangle);
    fixCollision(overlap, boundaryRectangle);
  }
}

/**
 * Calculate the overlap resulting from a collision between two elements.
 *
 * @param boundaryRectangle The boundary rectangle of the element that this element is
 * colliding with
 * @return An SDL_Point containing the x and y dimensions of the overlap
 */
SDL_Point Element::calculateOverlap(const SDL_Rect boundaryRectangle) const {
  SDL_Point overlap = {0, 0};

  // Right edge
  if (this->boundaryRectangle.x < boundaryRectangle.x) {
    overlap.x =
        (this->boundaryRectangle.x + this->boundaryRectangle.w) - boundaryRectangle.x;
  }
  // Left edge
  else {
    overlap.x = this->boundaryRectangle.x - (boundaryRectangle.x + boundaryRectangle.w);
    overlap.x = -overlap.x;
  }
  // Bottom edge
  if (this->boundaryRectangle.y < boundaryRectangle.y) {
    overlap.y =
        (this->boundaryRectangle.y + this->boundaryRectangle.h) - boundaryRectangle.y;
  }
  // Top edge
  else {
    overlap.y = this->boundaryRectangle.y - (boundaryRectangle.y + boundaryRectangle.h);
    overlap.y = -overlap.y;
  }

  return overlap;
}

/**
 * Undo a collision between two elements by moving this element outside of the other
 * element.
 *
 * @param overlap An SDL_Point containing the x and y dimensions of the overlap
 * @param boundaryRectangle The boundary rectangle of the element that this element is
 * colliding with
 */
void Element::fixCollision(const SDL_Point overlap, const SDL_Rect boundaryRectangle) {
  if (overlap.x > 0 && overlap.y > 0) {
    if (overlap.x < overlap.y) {
      // Right
      if (this->boundaryRectangle.x < boundaryRectangle.x) {
        this->positionRelativeToParent.x -= overlap.x;
      }
      // Left
      else {
        this->positionRelativeToParent.x += overlap.x;
      }
    }
    else {
      // Bottom
      if (this->boundaryRectangle.y < boundaryRectangle.y) {
        this->positionRelativeToParent.y -= overlap.y;
      }
      // Top
      else {
        this->positionRelativeToParent.y += overlap.y;
      }
    }
    updatePosition();
  }
}
