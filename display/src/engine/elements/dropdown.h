#ifndef DROPDOWN_H
#define DROPDOWN_H

#include "button.h"
#include "composite_element.h"

class Dropdown : public CompositeElement {
public:
  Dropdown(struct DisplayGlobal displayGlobal, const SDL_Rect& boundaryRectangle);

  void addOption(std::unique_ptr<Button> newOptionButton);

private:
  void handleEventSelf(const SDL_Event& event) override;
};

#endif
