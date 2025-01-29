#ifndef FOOD_ITEM_H
#define FOOD_ITEM_H

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
void receiveFoodItem(struct FoodItem&, int);

void writeString(int, const std::string&);
std::string readString(int);

#endif
