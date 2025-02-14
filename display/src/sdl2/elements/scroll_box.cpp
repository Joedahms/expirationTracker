#define SCROLL_AMOUNT 4

#include <SDL2/SDL.h>
#include <glog/logging.h>
#include <iostream>
#include <memory>
#include <vector>

#include "../../../../food_item.h"
#include "../../sql_food.h"
#include "../display_global.h"
#include "../sdl_debug.h"
#include "panel.h"
#include "scroll_box.h"

ScrollBox::ScrollBox(struct DisplayGlobal displayGlobal) {
  this->displayGlobal  = displayGlobal;
  this->previousUpdate = std::chrono::steady_clock::now();
}

void ScrollBox::refreshPanels() {
  std::vector<FoodItem> allFoodItems = readAllFoodItems();

  this->panels.clear();
  for (auto& foodItem : allFoodItems) {
    std::vector<std::unique_ptr<Text>> texts;

    SDL_Color textColor = {0, 255, 0, 255};
    SDL_Rect rect       = {0, 0, 0, 0};

    // Name
    const char* foodItemName = foodItem.name.c_str();
    std::unique_ptr<Text> name =
        std::make_unique<Text>(this->displayGlobal, this->displayGlobal.futuramFontPath,
                               foodItemName, 24, textColor, rect);
    texts.push_back(std::move(name));

    // Expires
    std::unique_ptr<Text> expiration =
        std::make_unique<Text>(this->displayGlobal, this->displayGlobal.futuramFontPath,
                               " Expires: ", 24, textColor, rect);
    texts.push_back(std::move(expiration));

    // Month
    std::string expirationDateMonth =
        std::to_string(static_cast<unsigned>(foodItem.expirationDate.month()));
    std::unique_ptr<Text> month =
        std::make_unique<Text>(this->displayGlobal, this->displayGlobal.futuramFontPath,
                               expirationDateMonth.c_str(), 24, textColor, rect);
    texts.push_back(std::move(month));

    // Slash
    std::unique_ptr<Text> slash =
        std::make_unique<Text>(this->displayGlobal, this->displayGlobal.futuramFontPath,
                               "/", 24, textColor, rect);
    texts.push_back(std::move(slash));

    // Date
    std::string expirationDateDay =
        std::to_string(static_cast<unsigned>(foodItem.expirationDate.day()));
    std::unique_ptr<Text> day =
        std::make_unique<Text>(this->displayGlobal, this->displayGlobal.futuramFontPath,
                               expirationDateDay.c_str(), 24, textColor, rect);
    texts.push_back(std::move(day));

    // Slash
    std::unique_ptr<Text> slash2 =
        std::make_unique<Text>(this->displayGlobal, this->displayGlobal.futuramFontPath,
                               "/", 24, textColor, rect);
    texts.push_back(std::move(slash2));

    // Year
    std::string expirationDateYear =
        std::to_string(static_cast<int>(foodItem.expirationDate.year()));
    std::unique_ptr<Text> year =
        std::make_unique<Text>(this->displayGlobal, this->displayGlobal.futuramFontPath,
                               expirationDateYear.c_str(), 24, textColor, rect);
    texts.push_back(std::move(year));

    // Year
    std::string quantityString = std::to_string(static_cast<int>(foodItem.quantity));
    std::unique_ptr<Text> quantityText =
        std::make_unique<Text>(this->displayGlobal, this->displayGlobal.futuramFontPath,
                               quantityString.c_str(), 24, textColor, rect);
    texts.push_back(std::move(quantityText));

    std::unique_ptr<Panel> newPanel =
        std::make_unique<Panel>(this->displayGlobal, foodItem.id, rect, std::move(texts));
    addPanel(std::move(newPanel), this->rectangle);
  }
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
void ScrollBox::update() {
  for (auto& currPanel : this->panels) {
    currPanel->update();
  }

  this->currentUpdate = std::chrono::steady_clock::now();

  std::chrono::seconds updateDifference;
  updateDifference = std::chrono::duration_cast<std::chrono::seconds>(
      this->currentUpdate - this->previousUpdate);

  // 5 or more seconds since last update
  if (updateDifference.count() > 5) {
    refreshPanels();
    this->previousUpdate = this->currentUpdate;
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
  std::unique_ptr<Panel> newPanel = std::move(panel);
  SDL_Rect newPanelRect           = newPanel->getRectangle();
  newPanelRect.h                  = this->panelHeight;
  newPanelRect.w                  = containingRectangle.w;
  if (this->panels.size() == 0) {
    newPanelRect.y = this->topPanelPosition;
  }
  else {
    newPanelRect.y = this->panels.back()->getRectangle().y + this->panelHeight;
  }
  newPanel->setRectangle(newPanelRect);
  newPanel->update();
  newPanel->addBorder(1);
  this->panels.push_back(std::move(newPanel));
}

/**
 * Move all panels within the scroll box up.
 *
 * @param mousePosition Position of the mouse used to check if the mouse is within the
 * scrollbox or not
 * @return None
 */
void ScrollBox::scrollUp(const SDL_Point* mousePosition) {
  if (SDL_PointInRect(mousePosition, &this->rectangle) == false) {
    return;
  }
  this->topPanelPosition -= SCROLL_AMOUNT;
  for (auto& currPanel : this->panels) {
    SDL_Rect currPanelRectangle = currPanel->getRectangle();
    currPanelRectangle.y -= SCROLL_AMOUNT;
    currPanel->setRectangle(currPanelRectangle);
    currPanel->update();
  }
}

/**
 * Move all panels within the scroll box down.
 *
 * @param mousePosition Position of the mouse used to check if the mouse is within the
 * scrollbox or not
 * @return None
 */
void ScrollBox::scrollDown(const SDL_Point* mousePosition) {
  if (SDL_PointInRect(mousePosition, &this->rectangle) == false) {
    return;
  }
  this->topPanelPosition += SCROLL_AMOUNT;
  for (auto& currPanel : this->panels) {
    SDL_Rect currPanelRectangle = currPanel->getRectangle();
    currPanelRectangle.y += SCROLL_AMOUNT;
    currPanel->setRectangle(currPanelRectangle);
    currPanel->update();
  }
}

void ScrollBox::render() const {
  if (this->hasBorder) {
    renderBorder();
  }
  for (auto& x : this->panels) {
    x->render();
  }
}
