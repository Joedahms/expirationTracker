#ifndef FOOD_ITEM_H
#define FOOD_ITEM_H

#define START_SCAN "start scan"

#include <chrono>
#include <filesystem>
#include <memory>
#include <string>
#include <zmqpp/zmqpp.hpp>

#include "fooditem.pb.h"

enum class FoodCategories { unknown, unpackaged, packaged };

class FoodItem {
public:
  FoodItem(const std::filesystem::path& imagePath,
           const std::chrono::year_month_day& scanDate,
           const float& weight)
      : imagePath(imagePath), scanDate(scanDate), weight(weight) {}

  std::string foodCategoryToString(const FoodCategories&);
  FoodCategories foodCategoryFromString(const std::string&);
  std::string sendFoodItem(zmqpp::socket& socket, const FoodItem& foodItem);
  bool receiveFoodItem(zmqpp::socket& socket,
                       const std::string& response,
                       struct FoodItem& foodItem);

private:
  int id;
  std::filesystem::path imagePath;
  std::string name;
  FoodCategories category;
  std::chrono::year_month_day scanDate;
  std::chrono::year_month_day expirationDate;
  float weight;
  int quantity;

  FoodItemProto::FoodItem convertToProto(const FoodItem& foodItem);
  FoodItem convertFromProto(const FoodItemProto::FoodItem& protoFoodItem);
};

/*
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
*/

// void sendFoodItem(struct FoodItem, int);
// bool receiveFoodItem(struct FoodItem&, int, struct timeval);

// void writeString(int, const std::string&);
// std::string readString(int pipeToRead);
// std::string foodCategoryToString(const FoodCategories&);
// FoodCategories foodCategoryFromString(const std::string&);
// void printFoodItem(const FoodItem&);

/*
std::string sendFoodItem(zmqpp::socket& socket, const FoodItem& foodItem);
bool receiveFoodItem(zmqpp::socket& socket,
                     const std::string& response,
                     struct FoodItem& foodItem);

FoodItemProto::FoodItem convertToProto(const FoodItem& foodItem);
FoodItem convertFromProto(const FoodItemProto::FoodItem& protoFoodItem);
*/
#endif
