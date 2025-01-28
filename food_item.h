#ifndef FOOD_ITEM_H
#define FOOD_ITEM_H

#include <chrono>
#include <string>

struct FoodItem {
  std::string photoPath;
  std::string name;
  std::chrono::year_month_day scanDate;
  std::chrono::year_month_day expirationDate;
  float weight;
  std::string catagory;
  int quantity;
};

#endif
