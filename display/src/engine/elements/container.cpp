#include "container.h"

Container::Container() {}

Container::Container(const SDL_Rect& boundaryRectangle) {
  this->boundaryRectangle = boundaryRectangle;
  setupPosition(boundaryRectangle);
}

void Container::handleEventSelf(const SDL_Event& event) {}
