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
    : decreaseButton(displayGlobal, SDL_Rect{0, 0, 0, 0}, "-", this->DECREMENT_SETTING),
      increaseButton(displayGlobal, SDL_Rect{0, 0, 0, 0}, "+", this->INCREMENT_SETTING),
      settingValueText(displayGlobal,
                       displayGlobal.futuramFontPath,
                       "0",
                       24,
                       SDL_Color{0, 255, 0, 255},
                       SDL_Rect{0, 0, 0, 0}),
      settingId(settingId) {
  FoodItem foodItem  = readFoodItemById(this->settingId);
  this->settingValue = foodItem.quantity;
  this->settingValueText.setContent(std::to_string(this->settingValue));
}

/**
 * Check that the new ID is valid read the current setting value from the database.
 *
 * @param newSettingId The new ID to assign to the number setting
 * @return None
 */
void NumberSetting::setSettingId(const int& newSettingId) {
  if (newSettingId < 0) {
    LOG(FATAL) << "Attempt to set number setting id to invalid id";
  }
  else {
    this->settingId = newSettingId;
  }
  FoodItem foodItem  = readFoodItemById(this->settingId);
  this->settingValue = foodItem.quantity;
}

/**
 * Handle SDL_MOUSEBUTTONDOWN. Check if either of the buttons have been clicked.
 *
 * @param mousePosition The x,y position of the mouse when it was clicked
 * @return None
 */
void NumberSetting::handleMouseButtonDown(const SDL_Point& mousePosition) {
  SDL_Rect increaseRectangle = this->increaseButton.getRectangle();
  SDL_Rect decreaseRectangle = this->decreaseButton.getRectangle();

  if (SDL_PointInRect(&mousePosition, &decreaseRectangle) == true) {
    this->settingValue--;
  }
  else if (SDL_PointInRect(&mousePosition, &increaseRectangle) == true) {
    this->settingValue++;
  }
}

/**
 * Position buttons and text and update them. If the setting value stored in the database
 * is different than the one stored in the object, update the value in the database with
 * the local one. If not, set the current setting to the local value.
 *
 * @param None
 * @return None
 */
void NumberSetting::update() {
  // Decrease
  SDL_Rect decreaseButtonRect = decreaseButton.getRectangle();
  decreaseButtonRect.y        = this->rectangle.y;
  decreaseButtonRect.x        = this->rectangle.x;
  decreaseButton.setRectangle(decreaseButtonRect);

  // Value
  SDL_Rect settingValueTextRect = settingValueText.getRectangle();
  settingValueTextRect.y        = this->rectangle.y;
  settingValueTextRect.x        = decreaseButtonRect.x + decreaseButtonRect.w;
  settingValueText.setRectangle(settingValueTextRect);

  // Increase
  SDL_Rect increaseButtonRect = increaseButton.getRectangle();
  increaseButtonRect.y        = this->rectangle.y;
  increaseButtonRect.x        = settingValueTextRect.x + settingValueTextRect.w;
  increaseButton.setRectangle(increaseButtonRect);

  this->settingValueText.update();
  this->increaseButton.update();
  this->decreaseButton.update();

  FoodItem foodItem = readFoodItemById(this->settingId);
  if (foodItem.quantity != this->settingValue) {
    updateFoodItemQuantity(this->settingId, this->settingValue);
  }
  else {
    this->settingValueText.setContent(std::to_string(this->settingValue));
  }
}

void NumberSetting::render() const {
  this->settingValueText.render();
  this->increaseButton.render();
  this->decreaseButton.render();
}
