#ifndef FOOD_ITEM_H
#define FOOD_ITEM_H

#define START_SCAN "start scan"

#include <chrono>
#include <filesystem>
#include <memory>
#include <string>

enum class FoodCategories { unknown, unpackaged, packaged };

struct FoodItem {
  int id;
  std::filesystem::path imageDirectory;
  std::string name;
  enum FoodCategories category;
  std::chrono::year_month_day scanDate;
  std::chrono::year_month_day expirationDate;
  float weight;
  int quantity;
  std::filesystem::path absolutePath;
};

void sendFoodItem(struct FoodItem, int);
bool receiveFoodItem(struct FoodItem&, int, struct timeval);

void writeString(int, const std::string&);
std::string readString(int);
bool isPathLike(const std::string&);
std::string foodCategoryToString(const FoodCategories&);
FoodCategories foodCategoryFromString(const std::string&);
#endif
