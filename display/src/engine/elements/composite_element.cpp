#include "composite_element.h"
#include "element.h"

void CompositeElement::update() {
  for (const auto& element : this->children) {
    element->update();
  }
}

void CompositeElement::addElement(std::unique_ptr<Element> element) {
  this->children.push_back(std::move(element));
}

void CompositeElement::render() const {
  for (const auto& element : this->children) {
    element->render();
  }
}
