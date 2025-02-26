#include "container.h"

Container::Container() {}

Container::Container(const SDL_Rect& boundaryRectangle) {
  this->boundaryRectangle = boundaryRectangle;
}

void Container::updateSelf() {}
void Container::renderSelf() const {}
void Container::handleEventSelf(const SDL_Event& event) {}
