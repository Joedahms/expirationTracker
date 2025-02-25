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

ScrollBox::ScrollBox(struct DisplayGlobal displayGlobal,
                     const SDL_Rect& boundaryRectangle) {
  this->displayGlobal     = displayGlobal;
  this->previousUpdate    = std::chrono::steady_clock::now();
  this->boundaryRectangle = boundaryRectangle;
}

void ScrollBox::setPanelHeight(int panelHeight) { this->panelHeight = panelHeight; }

/**
 * Update all panels in the scroll box with food item information. Assumed that one panel
 * corresponds to one food item. Therefore there will be as many panels in the scroll box
 * as there are food items passed. Don't want to refresh panels constantly (will cause
 * program to crash), so only refresh on a set interval.
 */
void ScrollBox::updateSelf() {
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

/**
 * Check if the scrollbox is hovered. If it is hovered and the event is a mousewheel
 * event, scroll the panels up or down accordingly.
 *
 * @param event The SDL event that has occured
 * @return None
 */
void ScrollBox::handleEventSelf(const SDL_Event& event) {
  if (checkHovered() == false) {
    return;
  }

  if (event.type == SDL_MOUSEWHEEL) {
    if (event.wheel.y > 0) {
      scrollUp();
    }
    else if (event.wheel.y < 0) {
      scrollDown();
    }
  }
}

void ScrollBox::renderSelf() const {}

/**
 * Ensure that all panels have data consistent with what is in the database. Destroys all
 * panels and makes new ones.
 *
 * @param None
 * @return None
 */
void ScrollBox::refreshPanels() {
  std::vector<FoodItem> allFoodItems = readAllFoodItems();

  this->children.clear();
  SDL_Point relativePosition = {0, 0};
  for (auto& foodItem : allFoodItems) {
    std::unique_ptr<Panel> newPanel =
        std::make_unique<Panel>(this->displayGlobal, foodItem.id, relativePosition);
    relativePosition.y += panelHeight;

    newPanel->addFoodItem(foodItem, relativePosition);
    int borderThickness = 1;
    newPanel->addBorder(borderThickness);
    addElement(std::move(newPanel));
  }
}

/**
 * Move all panels within the scroll box up. Scroll amount is hardcoded.
 *
 * @param None
 * @return None
 */
void ScrollBox::scrollUp() {
  for (auto& currPanel : this->children) {
    SDL_Point currPanelRelativePosition = currPanel->getPositionRelativeToParent();
    currPanelRelativePosition.y -= this->scrollAmount;
    currPanel->setPositionRelativeToParent(currPanelRelativePosition);
  }
}

/**
 * Move all panels within the scroll box down. Scroll amount is hardcoded.
 *
 * @param None
 * @return None
 */
void ScrollBox::scrollDown() {
  for (auto& currPanel : this->children) {
    SDL_Point currPanelRelativePosition = currPanel->getPositionRelativeToParent();
    currPanelRelativePosition.y += this->scrollAmount;
    currPanel->setPositionRelativeToParent(currPanelRelativePosition);
  }
}
