#ifndef DROPDOWN_H
#define DROPDOWN_H

#include "button.h"
#include "composite_element.h"

class Dropdown : public CompositeElement {
public:
  Dropdown(struct DisplayGlobal displayGlobal,
           const SDL_Rect& boundaryRectangle,
           const std::string& titleContent);

  void addOption(std::unique_ptr<Button> newOption);

private:
  void handleEventSelf(const SDL_Event& event) override;
};

#endif
