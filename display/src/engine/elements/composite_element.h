#ifndef COMPOSITE_ELEMENT_H
#define COMPOSITE_ELEMENT_H

#include <vector>

#include "element.h"

/**
 * An element that contains other elements within it.
 */
class CompositeElement : public Element {
public:
  void addElement(std::unique_ptr<Element> element) override;
  void update() override;
  void render() const override;
  void handleEvent(const SDL_Event& event) override;

private:
  virtual void updateSelf();
  virtual void renderSelf() const                      = 0;
  virtual void handleEventSelf(const SDL_Event& event) = 0;

protected:
  std::vector<std::unique_ptr<Element>> children;
};

#endif
