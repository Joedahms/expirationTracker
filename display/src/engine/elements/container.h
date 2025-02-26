#ifndef CONTAINER_H
#define CONTAINER_H

#include "composite_element.h"

/**
 * A simple composite element that just contains other elements without adding any of its
 * own functionality. Useful as a root element of an element heirarchy.
 */
class Container : public CompositeElement {
public:
  Container();
  Container(const SDL_Rect& boundaryRectangle);
  void updateSelf() override;
  void renderSelf() const override;
  void handleEventSelf(const SDL_Event& event) override;
};

#endif
