#include "obstacle.h"

Obstacle::Obstacle(struct DisplayGlobal displayGlobal,
                   const SDL_Rect& boundaryRectangle) {
  this->displayGlobal = displayGlobal;
  setupPosition(boundaryRectangle);
  this->velocity.x = -3;
}

void Obstacle::update() {
  if (parent) {
    hasParentUpdate();
  }

  int windowWidth;
  SDL_GetWindowSize(this->displayGlobal.window, &windowWidth, NULL);

  if (this->boundaryRectangle.x <= -(this->boundaryRectangle.w)) {
    this->positionRelativeToParent.x = windowWidth;
    updatePosition();
  }
}

void Obstacle::handleEvent(const SDL_Event& event) {}

void Obstacle::render() const {
  SDL_SetRenderDrawColor(this->displayGlobal.renderer, 255, 0, 0, 255);
  SDL_RenderFillRect(this->displayGlobal.renderer, &this->boundaryRectangle);
}
