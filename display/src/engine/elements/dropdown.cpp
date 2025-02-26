#include "dropdown.h"

Dropdown::Dropdown(struct DisplayGlobal displayGlobal,
                   const SDL_Rect& boundaryRectangle) {
  this->displayGlobal = displayGlobal;

  setupPosition(boundaryRectangle);
}

void Dropdown::addOption(std::unique_ptr<Button> newOption) {
  addElement(std::move(newOption));
}

void Dropdown::handleEventSelf(const SDL_Event& event) {}
