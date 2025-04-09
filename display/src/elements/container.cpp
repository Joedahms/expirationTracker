#include "container.h"

Container::Container() {}

Container::Container(const SDL_Rect& boundaryRectangle) {
  this->boundaryRectangle = boundaryRectangle;
  setupPosition(boundaryRectangle);
}

void Container::update() {
  updateSelf();

  for (const auto& element : this->children) {
    element->update();
    if (element->getFixed()) {
      continue;
    }
    if (element->getScreenBoundX()) {
      checkElementPositionX(element);
    }
    if (element->getScreenBoundY()) {
      checkElementPositionY(element);
    }
  }
}

void Container::checkElementPositionX(std::shared_ptr<Element> element) {
  Velocity elementVelocity                  = element->getVelocity();
  int elementBorderThickness                = element->getBorderThickness();
  SDL_Point elementPositionRelativeToParent = element->getPositionRelativeToParent();
  SDL_Rect elementBoundaryRectangle         = element->getBoundaryRectangle();

  // Left
  if (elementPositionRelativeToParent.x - elementBorderThickness < 0) {
    elementVelocity.x = 0;
    element->setVelocity(elementVelocity);

    elementPositionRelativeToParent.x = elementBorderThickness;
    element->setPositionRelativeToParent(elementPositionRelativeToParent);
  }

  // Right
  int elementRightEdge = elementPositionRelativeToParent.x + elementBoundaryRectangle.w +
                         elementBorderThickness;
  if (elementRightEdge > this->boundaryRectangle.x + this->boundaryRectangle.w) {
    elementVelocity.x = 0;
    element->setVelocity(elementVelocity);

    elementPositionRelativeToParent.x =
        this->boundaryRectangle.x + this->boundaryRectangle.w -
        elementBoundaryRectangle.w - elementBorderThickness;
    element->setPositionRelativeToParent(elementPositionRelativeToParent);
  }
}

void Container::checkElementPositionY(std::shared_ptr<Element> element) {
  Velocity elementVelocity                  = element->getVelocity();
  int elementBorderThickness                = element->getBorderThickness();
  SDL_Point elementPositionRelativeToParent = element->getPositionRelativeToParent();
  SDL_Rect elementBoundaryRectangle         = element->getBoundaryRectangle();

  // Top
  if (elementPositionRelativeToParent.y - elementBorderThickness < 0) {
    elementVelocity.y = 0;
    element->setVelocity(elementVelocity);

    elementPositionRelativeToParent.y = elementBorderThickness;
    element->setPositionRelativeToParent(elementPositionRelativeToParent);
  }

  // Bottom
  int elementBottomEdge = elementPositionRelativeToParent.y + elementBoundaryRectangle.h +
                          elementBorderThickness;
  if (elementBottomEdge > this->boundaryRectangle.y + this->boundaryRectangle.h) {
    elementVelocity.y = 0;
    element->setVelocity(elementVelocity);

    elementPositionRelativeToParent.y =
        this->boundaryRectangle.y + this->boundaryRectangle.h -
        elementBoundaryRectangle.h - elementBorderThickness;
    element->setPositionRelativeToParent(elementPositionRelativeToParent);
  }
}

void Container::handleEventSelf(const SDL_Event& event) {}
