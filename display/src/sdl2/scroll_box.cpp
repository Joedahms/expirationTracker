#include <SDL2/SDL.h>
#include <iostream>
#include <memory>
#include <vector>

#include "../../../food_item.h"
#include "../sql_food.h"
#include "display_global.h"
#include "panel.h"
#include "scroll_box.h"

ScrollBox::ScrollBox(struct DisplayGlobal dg) : displayGlobal(dg) {}

void ScrollBox::setPanelHeight(int panelHeight) { this->panelHeight = panelHeight; }

void ScrollBox::updatePanels(std::vector<FoodItem> allFoodItems) {
  this->panels.clear();
  for (auto& foodItem : allFoodItems) {
    std::vector<std::unique_ptr<Text>> texts;

    const char* foodItemName = foodItem.name.c_str();

    SDL_Color textColor = {0, 255, 0, 255};
    SDL_Rect rect       = {0, 0, 0, 0};
    std::unique_ptr<Text> text =
        std::make_unique<Text>(this->displayGlobal, this->displayGlobal.futuramFontPath,
                               foodItemName, 24, textColor, rect);

    texts.push_back(std::move(text));

    std::unique_ptr<Panel> newPanel = std::make_unique<Panel>(rect, std::move(texts));
    addPanel(std::move(newPanel));
  }
}

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
  newPanel->updateElementPositions();
  this->panels.push_back(std::move(newPanel));
}

void ScrollBox::render() {
  for (auto& x : this->panels) {
    x->render();
  }
}
