#include <SDL2/SDL.h>
#include <iostream>
#include <memory>
#include <vector>

#include "panel.h"
#include "scroll_box.h"

void ScrollBox::setPanelHeight(int panelHeight) { this->panelHeight = panelHeight; }

void ScrollBox::addPanel(std::unique_ptr<Panel> panel) {
  std::unique_ptr newPanel = std::move(panel);
  SDL_Rect newPanelRect    = newPanel->getRectangle();
  newPanelRect.h           = this->panelHeight;
  if (this->panels.size() == 0) {
    newPanelRect.y = this->rectangle.y;
  }
  else {
    newPanelRect.y = this->panels.back()->getRectangle().y + this->panelHeight;
  }
  newPanel->setRectangle(newPanelRect);
  newPanel->updateElements();
  this->panels.push_back(std::move(newPanel));
}

void ScrollBox::render() {
  for (auto& x : this->panels) {
    x->render();
  }
}
