#ifndef COMPOSITE_ELEMENT_H
#define COMPOSITE_ELEMENT_H

#include <vector>

#include "element.h"

class CompositeElement : public Element {
public:
  void update() override;
  void addElement(std::unique_ptr<Element> element) override;
  void render() const override;

private:
  std::vector<std::unique_ptr<Element>> children;
};

#endif
