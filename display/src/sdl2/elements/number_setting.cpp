#include <sqlite3.h>

#include "../../../../food_item.h"
#include "../../sql_food.h"
#include "number_setting.h"

NumberSetting::NumberSetting() {}

void NumberSetting::setSettingId(const int& newSettingId) {
  if (newId < 0) {
    LOG(FATAL) << "Attempt to set number setting id to invalid id";
  }
  else {
    this->settingId = newSettingId;
  }
}

void NumberSetting::update() { FoodItem foodItem = readFoodItemById(this->settingId); }

void NumberSetting::render() const {}
