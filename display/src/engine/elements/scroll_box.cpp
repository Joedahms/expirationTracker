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

/*
ScrollBox::ScrollBox(struct DisplayGlobal displayGlobal) {
  this->displayGlobal  = displayGlobal;
  this->previousUpdate = std::chrono::steady_clock::now();
}
*/

/**
 * Ensure that all panels have data consistent with what is in the database. Destroys all
 * panels and makes new ones.
 *
 * @param None
 * @return None
 */
/*
void ScrollBox::refreshPanels() {
  std::vector<FoodItem> allFoodItems = readAllFoodItems();

  this->panels.clear();
  for (auto& foodItem : allFoodItems) {
    std::unique_ptr<Panel> newPanel =
        std::make_unique<Panel>(this->displayGlobal, foodItem.id);

    newPanel->addFoodItem(foodItem);
    addPanel(std::move(newPanel), this->rectangle);
  }
}
*/

// void ScrollBox::setPanelHeight(int panelHeight) { this->panelHeight = panelHeight; }

/**
 * Add a new panel to the scroll box. Adds the new panel directly below the previously
 * lowest panel.
 *
 * @param The new panel to add
 * @param The rectangle of the element that the panel is being added within
 * @return None
 */
/*
void ScrollBox::addPanel(std::unique_ptr<Panel> panel, SDL_Rect containingRectangle) {
  std::unique_ptr<Panel> newPanel = std::move(panel);
  SDL_Rect newPanelRect           = newPanel->getRectangle();
  newPanelRect.h                  = this->panelHeight;
  newPanelRect.w                  = containingRectangle.w;
  newPanelRect.x                  = containingRectangle.x;

  // Set Y position. Could be broken out into positionPanel function.
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
*/

/**
 * Handle SDL_MOUSEBUTTONDOWN. If the mouse was clicked within a panel, tell that panel to
 * handle the click event.
 *
 * @param mousePosition x,y position of the mouse when it was clicked
 * @return None
 */
/*
void ScrollBox::handleMouseButtonDown(const SDL_Point& mousePosition) {
  for (auto& currPanel : this->panels) {
    SDL_Rect currPanelRectangle = currPanel->getRectangle();
    if (SDL_PointInRect(&mousePosition, &currPanelRectangle) == true) {
      currPanel->handleMouseButtonDown(mousePosition);
      return;
    }
  }
}
*/

/**
 * Update all panels in the scroll box with food item information. Assumed that one panel
 * corresponds to one food item. Therefore there will be as many panels in the scroll box
 * as there are food items passed. Don't want to refresh panels constantly (will cause
 * program to crash), so only refresh on a set interval.
 *
 * @param allFoodItems A vector of all the food items to put in panels
 * @return None
 */
/*
void ScrollBox::update() {
  for (auto& currPanel : this->panels) {
    currPanel->update();
  }

  // Get time since last update
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
*/

/**
 * Move all panels within the scroll box up.
 *
 * @param mousePosition Position of the mouse used to check if the mouse is within the
 * scrollbox or not
 * @return None
 */
/*
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
*/

/**
 * Move all panels within the scroll box down.
 *
 * @param mousePosition Position of the mouse used to check if the mouse is within the
 * scrollbox or not
 * @return None
 */
/*
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
*/
