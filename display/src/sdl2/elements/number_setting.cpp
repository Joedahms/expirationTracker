#include <glog/logging.h>
#include <sqlite3.h>

#include "../../../../food_item.h"
#include "../../sql_food.h"
#include "../sdl_debug.h"
#include "number_setting.h"

NumberSetting::NumberSetting(struct DisplayGlobal displayGlobal, int settingId)
    : decreaseButton(displayGlobal, SDL_Rect{0, 0, 0, 0}, "-", DECREMENT_SETTING),
      increaseButton(displayGlobal, SDL_Rect{0, 0, 0, 0}, "+", INCREMENT_SETTING),
      settingValueText(displayGlobal,
                       displayGlobal.futuramFontPath,
                       "0",
                       24,
                       SDL_Color{0, 255, 0, 255},
                       SDL_Rect{0, 0, 0, 0}),
      settingId(settingId) {
  FoodItem foodItem  = readFoodItemById(this->settingId);
  this->settingValue = foodItem.quantity;
}

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

void NumberSetting::update() {
  SDL_Rect decreaseButtonRect = decreaseButton.getRectangle();
  decreaseButtonRect.y        = this->rectangle.y;
  decreaseButtonRect.x        = this->rectangle.x;
  decreaseButton.setRectangle(decreaseButtonRect);

  SDL_Rect settingValueTextRect = settingValueText.getRectangle();
  settingValueTextRect.y        = this->rectangle.y;
  settingValueTextRect.x        = decreaseButtonRect.x + decreaseButtonRect.w;
  settingValueText.setRectangle(settingValueTextRect);

  SDL_Rect increaseButtonRect = increaseButton.getRectangle();
  increaseButtonRect.y        = this->rectangle.y;
  increaseButtonRect.x        = settingValueTextRect.x + settingValueTextRect.w;
  increaseButton.setRectangle(increaseButtonRect);

  this->settingValueText.update();
  this->increaseButton.update();
  this->decreaseButton.update();

  FoodItem foodItem = readFoodItemById(this->settingId);
  if (foodItem.quantity != this->settingValue) {
    // update database
  }
}

void NumberSetting::render() const {
  this->settingValueText.render();
  this->increaseButton.render();
  this->decreaseButton.render();
}
