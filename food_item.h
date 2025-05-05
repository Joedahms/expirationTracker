#ifndef FOOD_ITEM_H
#define FOOD_ITEM_H

#include <chrono>
#include <filesystem>
#include <memory>
#include <string>
#include <zmqpp/zmqpp.hpp>

#include "fooditem.pb.h"
#include "logger.h"

enum class FoodCategories { unknown, unpackaged, packaged };

class FoodItem {
public:
  FoodItem() = default;
  FoodItem(const std::filesystem::path& imagePath,
           const std::chrono::year_month_day& scanDate)
      : imagePath(imagePath), scanDate(scanDate) {}

  std::string categoryToString() const;

  void logToFile(const Logger& logger) const;
  int getId() const;
  std::filesystem::path getImagePath() const;
  std::string getName() const;
  FoodCategories getCategory() const;
  std::chrono::year_month_day getScanDate() const;
  std::chrono::year_month_day getExpirationDate() const;
  int getQuantity() const;

  void setId(const int& id);
  void setImagePath(const std::filesystem::path& imagePath);
  void setName(const std::string& name);
  void setCategory(const FoodCategories& category);
  void setScanDate(const std::chrono::year_month_day& scanDate);
  void setExpirationDate(const std::chrono::year_month_day& expirationDate);
  void setQuantity(const int& quantity);

private:
  int id;
  std::filesystem::path imagePath;
  std::string name;
  FoodCategories category;
  std::chrono::year_month_day scanDate;
  std::chrono::year_month_day expirationDate;
  int quantity;
};

FoodCategories foodCategoryFromString(const std::string&);
std::string sendFoodItem(zmqpp::socket& socket, const FoodItem& foodItem, Logger& logger);
bool receiveFoodItem(zmqpp::socket& socket,
                     const std::string& response,
                     struct FoodItem& foodItem);
FoodItemProto::FoodItem convertToProto(const FoodItem& foodItem);
FoodItem convertFromProto(const FoodItemProto::FoodItem& protoFoodItem);

#endif
