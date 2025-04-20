#ifndef CONTAINER_H
#define CONTAINER_H

#include "composite_element.h"

/**
 * A simple composite element that just contains other elements without adding any of its
 * own functionality. Useful as a root element of an element heirarchy.
 */
class Container : public CompositeElement {
public:
  Container(const struct DisplayGlobal& displayGlobal,
            const std::string& logFile,
            const SDL_Rect boundaryRectangle);
  void update() override;
  void handleEventSelf(const SDL_Event& event) override;

private:
  void checkElementPositionX(std::shared_ptr<Element> element);
  void checkElementPositionY(std::shared_ptr<Element> element);
};

#endif
