#include "dropdown.h"

Dropdown::Dropdown(struct DisplayGlobal displayGlobal,
                   const SDL_Rect& boundaryRectangle) {
  this->displayGlobal = displayGlobal;

  this->positionRelativeToParent.x = boundaryRectangle.x;
  this->positionRelativeToParent.y = boundaryRectangle.y;

  this->boundaryRectangle   = boundaryRectangle;
  this->boundaryRectangle.x = 0;
  this->boundaryRectangle.y = 0;
}

void Dropdown::addOption(std::unique_ptr<Button> newOptionButton) {
  if (this->children.size() == 0) {
    newOptionButton->setPositionRelativeToParent(this->positionRelativeToParent);
  }
  /*
  else {
    SDL_Point bottomRelative    = this->children.back()->getPositionRelativeToParent();
    SDL_Rect bottomBoundary     = this->children.back()->getBoundaryRectangle();
    SDL_Point newBottomRelative = {0, 0};
    newBottomRelative.x         = bottomRelative.x + bottomBoundary.h;
    newOptionButton->setPositionRelativeToParent(newBottomRelative);
  }
*/
  addElement(std::move(newOptionButton));
}

void Dropdown::handleEventSelf(const SDL_Event& event) {}
