#include <SDL2/SDL.h>
#include <iostream>
#include <memory>
#include <vector>

#include "../../../food_item.h"
#include "../sql_food.h"
#include "display_global.h"
#include "panel.h"
#include "scroll_box.h"

ScrollBox::ScrollBox(struct DisplayGlobal displayGlobal) {
  this->displayGlobal = displayGlobal;
}

void ScrollBox::setPanelHeight(int panelHeight) { this->panelHeight = panelHeight; }

/**
 * Update all panels in the scroll box with food item information. Assumed that one panel
 * corresponds to one food item. Therefore there will be as many panels in the scroll box
 * as there are food items passed.
 *
 * @param allFoodItems A vector of all the food items to put in panels
 * @return None
 */
void ScrollBox::updatePanels(std::vector<FoodItem> allFoodItems) {
  this->panels.clear();
  for (auto& foodItem : allFoodItems) {
    std::vector<std::unique_ptr<Text>> texts;

    SDL_Color textColor = {0, 255, 0, 255};
    SDL_Rect rect       = {0, 0, 0, 0};

    const char* foodItemName = foodItem.name.c_str();
    std::unique_ptr<Text> name =
        std::make_unique<Text>(this->displayGlobal, this->displayGlobal.futuramFontPath,
                               foodItemName, 24, textColor, rect);
    texts.push_back(std::move(name));

    std::unique_ptr<Text> expiration =
        std::make_unique<Text>(this->displayGlobal, this->displayGlobal.futuramFontPath,
                               " Expires: ", 24, textColor, rect);
    texts.push_back(std::move(expiration));

    std::string expirationDateMonth =
        std::to_string(static_cast<unsigned>(foodItem.expirationDate.month()));
    std::unique_ptr<Text> month =
        std::make_unique<Text>(this->displayGlobal, this->displayGlobal.futuramFontPath,
                               expirationDateMonth.c_str(), 24, textColor, rect);
    texts.push_back(std::move(month));

    std::unique_ptr<Text> slash =
        std::make_unique<Text>(this->displayGlobal, this->displayGlobal.futuramFontPath,
                               "/", 24, textColor, rect);
    texts.push_back(std::move(slash));

    std::string expirationDateDay =
        std::to_string(static_cast<unsigned>(foodItem.expirationDate.day()));
    std::unique_ptr<Text> day =
        std::make_unique<Text>(this->displayGlobal, this->displayGlobal.futuramFontPath,
                               expirationDateDay.c_str(), 24, textColor, rect);
    texts.push_back(std::move(day));

    std::unique_ptr<Text> slash2 =
        std::make_unique<Text>(this->displayGlobal, this->displayGlobal.futuramFontPath,
                               "/", 24, textColor, rect);
    texts.push_back(std::move(slash2));

    std::string expirationDateYear =
        std::to_string(static_cast<int>(foodItem.expirationDate.year()));
    std::unique_ptr<Text> year =
        std::make_unique<Text>(this->displayGlobal, this->displayGlobal.futuramFontPath,
                               expirationDateYear.c_str(), 24, textColor, rect);
    texts.push_back(std::move(year));

    std::unique_ptr<Panel> newPanel =
        std::make_unique<Panel>(this->displayGlobal, rect, std::move(texts));
    addPanel(std::move(newPanel), this->rectangle);
  }
}

/**
 * Add a new panel to the scroll box. Adds the new panel directly below the previously
 * lowest panel.
 *
 * @param The new panel to add
 * @return None
 */
void ScrollBox::addPanel(std::unique_ptr<Panel> panel, SDL_Rect containingRectangle) {
  std::unique_ptr newPanel = std::move(panel);
  SDL_Rect newPanelRect    = newPanel->getRectangle();
  newPanelRect.h           = this->panelHeight;
  newPanelRect.w           = containingRectangle.w;
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

void ScrollBox::render() const {
  if (this->hasBorder) {
    renderBorder();
  }
  for (auto& x : this->panels) {
    x->render();
  }
}
