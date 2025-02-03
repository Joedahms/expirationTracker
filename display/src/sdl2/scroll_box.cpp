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
  this->panels.push_back(std::move(newPanel));

  /*
  int count = 0;
  for (int i = 0; i < this->panels.size(); i++) {
    std::cout << "Y position " << i << ": " << yPosition << std::endl;
    SDL_Rect updatedRectangle = this->panels[i]->getRectangle();
    updatedRectangle.y        = yPosition;
    this->panels[i]->updateRectangle(updatedRectangle);
    yPosition += spacing;
  }
  for (auto& currPanel : this->panels) {
    std::cout << "Y position " << count << ": " << yPosition << std::endl;
    SDL_Rect updatedRectangle = currPanel->getRectangle();
    updatedRectangle.y        = yPosition;
    currPanel->updateRectangle(updatedRectangle);
    yPosition += spacing;
    count++;
  }
  */
}

void ScrollBox::render() {
  for (auto& x : this->panels) {
    x->render();
  }
}
