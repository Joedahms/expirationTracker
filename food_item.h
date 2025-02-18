#ifndef FOOD_ITEM_H
#define FOOD_ITEM_H

#define START_SCAN "start scan"

#include <chrono>
#include <filesystem>
#include <memory>
#include <string>

struct FoodItem {
  int id;
  std::filesystem::path imageDirectory;
  std::string name;
  std::string category;
  std::chrono::year_month_day scanDate;
  std::chrono::year_month_day expirationDate;
  float weight;
  int quantity;
};

void sendFoodItem(struct FoodItem, int);
bool receiveFoodItem(struct FoodItem&, int, struct timeval);

void writeString(int, const std::string&);
std::string readString(int);
bool isPathLike(const std::string&);
#endif
