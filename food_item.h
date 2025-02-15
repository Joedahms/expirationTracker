#ifndef FOOD_ITEM_H
#define FOOD_ITEM_H

#define START_SCAN "start scan"

#include <chrono>
#include <memory>
#include <string>

struct FoodItem {
  std::string photoPath;
  std::string name;
  std::string catagory;
  std::chrono::year_month_day scanDate;
  std::chrono::year_month_day expirationDate;
  float weight;
  int quantity;
};

void sendFoodItem(struct FoodItem, int);
bool receiveFoodItem(struct FoodItem&, int, const struct timeval);

void writeString(int, const std::string&);
std::string readString(int);
bool isPathLike(const std::string&);
#endif
