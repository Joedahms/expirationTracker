#include <sqlite3.h>

#include "../../../../food_item.h"
#include "../../sql_food.h"
#include "number_setting.h"

NumberSetting::NumberSetting() {
  std::vector<FoodItem> allFoodItems = readAllFoodItems();
}

void NumberSetting::update() {}

void NumberSetting::render() const {}
