#ifndef CONTAINER_H
#define CONTAINER_H

#include "composite_element.h"

class Container : public CompositeElement {
public:
  void updateSelf() override;
  void renderSelf() const override;
  void handleEventSelf(const SDL_Event& event) override;
};

#endif
