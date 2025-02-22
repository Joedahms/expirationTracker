#include "composite_element.h"
#include "element.h"

void CompositeElement::update() {
  updateSelf();
  for (const auto& element : this->children) {
    element->update();
  }
}

void CompositeElement::addElement(std::unique_ptr<Element> element) {
  Element* childPointer = element.get();
  this->children.push_back(std::move(element));
  childPointer->setParent(this);
}

void CompositeElement::render() const {
  renderSelf();
  for (const auto& element : this->children) {
    element->render();
  }
}

void CompositeElement::handleEvent(const SDL_Event& event) {
  handleEventSelf(event);
  for (const auto& element : this->children) {
    element->handleEvent(event);
  }
}

void CompositeElement::updateSelf() {
  if (parent) {
    if (this->centerWithinParent) {
      if (checkCenterVertical() == false) {
        centerVertical();
      }
      if (checkCenterHorizontal() == false) {
        centerHorizontal();
      }
    }
    if (this->centerVerticalWithinParent) {
      if (checkCenterVertical() == false) {
        centerVertical();
      }
    }
    else if (this->centerHorizontalWithinParent) {
      if (checkCenterHorizontal() == false) {
        centerHorizontal();
      }
    }

    SDL_Rect parentBoundaryRectangle = parent->getBoundaryRectangle();
    this->boundaryRectangle.x =
        parentBoundaryRectangle.x + this->positionRelativeToParent.x;
    this->boundaryRectangle.y =
        parentBoundaryRectangle.y + this->positionRelativeToParent.y;
  }
}
