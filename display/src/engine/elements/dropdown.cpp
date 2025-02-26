#include "dropdown.h"

Dropdown::Dropdown(struct DisplayGlobal displayGlobal,
                   const SDL_Rect& boundaryRectangle,
                   const std::string& titleContent) {
  this->displayGlobal = displayGlobal;

  setupPosition(boundaryRectangle);

  std::unique_ptr<Text> title = std::make_unique<Text>(
      this->displayGlobal, SDL_Rect{0, 0, 0, 0}, this->displayGlobal.futuramFontPath,
      titleContent, 24, SDL_Color{0, 255, 0, 255});
  addElement(std::move(title));
}

void Dropdown::addOption(std::unique_ptr<Button> newOption) {
  SDL_Point newOptionRelativePosition = newOption->getPositionRelativeToParent();
  SDL_Rect lowestOptionRectangle      = this->children.back()->getBoundaryRectangle();
  newOptionRelativePosition.y         = lowestOptionRectangle.h + lowestOptionRectangle.y;
  newOption->setPositionRelativeToParent(newOptionRelativePosition);
  addElement(std::move(newOption));
}

void Dropdown::handleEventSelf(const SDL_Event& event) {}
