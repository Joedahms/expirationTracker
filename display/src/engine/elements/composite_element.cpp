#include "composite_element.h"
#include "element.h"

void CompositeElement::update() {
  updateSelf();
  for (const auto& element : this->children) {
    element->update();
  }
}

void CompositeElement::addElement(std::unique_ptr<Element> element) {
  this->children.push_back(std::move(element));
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
