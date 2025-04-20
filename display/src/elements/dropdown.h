#ifndef DROPDOWN_H
#define DROPDOWN_H

#include "button.h"

class CompositeElement;

class Dropdown : public CompositeElement {
public:
  Dropdown(const struct DisplayGlobal& displayGlobal,
           const std::string& logFile,
           const SDL_Rect boundaryRectangle,
           const std::string& titleContent);

  void addOption(std::shared_ptr<Button> newOption);

private:
  void handleEventSelf(const SDL_Event& event) override;
};

#endif
