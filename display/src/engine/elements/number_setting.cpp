#include <glog/logging.h>
#include <iostream>
#include <sqlite3.h>

#include "../../../../food_item.h"
#include "../../sql_food.h"
#include "../sdl_debug.h"
#include "number_setting.h"

/**
 * Initialize the setting buttons and the text between them. Get the current value of the
 * setting from the database and set class members accordingly.
 *
 * @param displayGlobal
 * @param settingId The primary key of the food item corresponding to this object
 */
NumberSetting::NumberSetting(struct DisplayGlobal displayGlobal, int settingId)
    : settingId(settingId) {
  std::unique_ptr<Button> decreaseButton = std::make_unique<Button>(
      displayGlobal, SDL_Rect{0, 0, 0, 0}, "-", [this]() { this->settingValue--; });
  addElement(std::move(decreaseButton));

  std::unique_ptr<Text> text =
      std::make_unique<Text>(displayGlobal, displayGlobal.futuramFontPath, "0", 24,
                             SDL_Color{0, 255, 0, 255}, SDL_Rect{0, 0, 0, 0});
  addElement(std::move(text));

  std::unique_ptr<Button> increaseButton = std::make_unique<Button>(
      displayGlobal, SDL_Rect{0, 0, 0, 0}, "+", [this]() { this->settingValue++; });
  addElement(std::move(increaseButton));

  FoodItem foodItem  = readFoodItemById(this->settingId);
  this->settingValue = foodItem.quantity;
  this->children[1]->setContent(std::to_string(this->settingValue));
}

void NumberSetting::updateSelf() {
  for (int i = 0; i < this->children.size(); i++) {
    SDL_Point childRelativePosition = this->children[i]->getPositionRelativeToParent();
    childRelativePosition.y         = this->positionRelativeToParent.y;
    if (i == 0) {
      childRelativePosition.x = this->positionRelativeToParent.x;
    }
    else {
      SDL_Point leftRelativePosition =
          this->children[i - 1]->getPositionRelativeToParent();
      SDL_Rect leftBoundaryRectangle = this->children[i - 1]->getBoundaryRectangle();
      childRelativePosition.x        = leftRelativePosition.x + leftBoundaryRectangle.w;
    }
    this->children[i]->setPositionRelativeToParent(childRelativePosition);
  }

  FoodItem foodItem = readFoodItemById(this->settingId);
  if (foodItem.quantity != this->settingValue) {
    updateFoodItemQuantity(this->settingId, this->settingValue);
  }
  else {
    this->children[1]->setContent(std::to_string(this->settingValue));
  }
}

void NumberSetting::renderSelf() const {}
void NumberSetting::handleEventSelf(const SDL_Event& event) {}
