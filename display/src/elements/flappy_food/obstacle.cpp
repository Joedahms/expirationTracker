#include "obstacle.h"

Obstacle::Obstacle(const struct DisplayGlobal& displayGlobal,
                   const std::string& logFile,
                   const SDL_Rect boundaryRectangle)
    : Element(displayGlobal, logFile, boundaryRectangle) {
  this->screenBoundX = false;
  this->canCollide   = true;
}

void Obstacle::handleEvent(const SDL_Event& event) {}

void Obstacle::render() const {
  SDL_SetRenderDrawColor(this->displayGlobal.renderer, 255, 0, 0, 255);
  SDL_RenderFillRect(this->displayGlobal.renderer, &this->boundaryRectangle);
}
