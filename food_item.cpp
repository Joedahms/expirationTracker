#include <filesystem>
#include <glog/logging.h>
#include <iostream>
#include <memory>
#include <sys/select.h>
#include <unistd.h>
#include <vector>

#include "food_item.h"
#include "fooditem.pb.h"

/**
 * Convert a food category enum to string
 *
 * @param category Enum to convert
 * @return The string version
 */
std::string FoodItem::categoryToString() const {
  switch (this->category) {
  case FoodCategories::unknown:
    return "unknown";
  case FoodCategories::unpackaged:
    return "unpackaged";
  case FoodCategories::packaged:
    return "packaged";
  default:
    return "invalid";
  }
}

void FoodItem::logToFile(const Logger& logger) const {
  logger.log("======== FoodItem Details ========");
  logger.log("ID: " + std::to_string(this->id));
  logger.log("Name: " + this->name);
  logger.log("ImagePath: " + this->imagePath.string());

  logger.log("Category: " + categoryToString());

  auto scan = this->scanDate;
  logger.log("Scan Date: " + std::to_string(static_cast<int>(scan.year())) + "-" +
             std::to_string(static_cast<unsigned>(scan.month())) + "-" +
             std::to_string(static_cast<unsigned>(scan.day())));

  auto exp = this->expirationDate;
  logger.log("Expiration Date: " + std::to_string(static_cast<int>(exp.year())) + "-" +
             std::to_string(static_cast<unsigned>(exp.month())) + "-" +
             std::to_string(static_cast<unsigned>(exp.day())));

  logger.log("Weight: " + std::to_string(this->weight) + " kg");
  logger.log("Quantity: " + std::to_string(this->quantity));
  logger.log("==================================");
}

int FoodItem::getId() const { return this->id; }
std::filesystem::path FoodItem::getImagePath() const { return this->imagePath; }
std::string FoodItem::getName() const { return this->name; }
FoodCategories FoodItem::getCategory() const { return this->category; }
std::chrono::year_month_day FoodItem::getScanDate() const { return this->scanDate; }
std::chrono::year_month_day FoodItem::getExpirationDate() const {
  return this->expirationDate;
}
float FoodItem::getWeight() const { return this->weight; }
int FoodItem::getQuantity() const { return this->quantity; }

void FoodItem::setId(const int& id) { this->id = id; }
void FoodItem::setImagePath(const std::filesystem::path& imagePath) {
  this->imagePath = imagePath;
}
void FoodItem::setName(const std::string& name) { this->name = name; }
void FoodItem::setCategory(const FoodCategories& category) { this->category = category; }
void FoodItem::setScanDate(const std::chrono::year_month_day& scanDate) {
  this->scanDate = scanDate;
}
void FoodItem::setExpirationDate(const std::chrono::year_month_day& expirationDate) {
  this->expirationDate = expirationDate;
}
void FoodItem::setWeight(const float& weight) { this->weight = weight; }
void FoodItem::setQuantity(const int& quantity) { this->quantity = quantity; }

/**
 * Print out FoodItem
 *
 * @param item food item to print
 */

std::string sendFoodItem(zmqpp::socket& socket, const FoodItem& foodItem) {
  FoodItemProto::FoodItem protoFoodItem = convertToProto(foodItem);
  std::string serializedString;
  protoFoodItem.SerializeToString(&serializedString);
  socket.send(serializedString);
  std::string response;
  socket.receive(response);
  return response;
}

bool receiveFoodItem(zmqpp::socket& socket,
                     const std::string& response,
                     struct FoodItem& foodItem) {
  bool received = false;
  std::string requestString;
  received = socket.receive(requestString, true);

  if (received) {
    FoodItemProto::FoodItem protoFoodItem;
    protoFoodItem.ParseFromString(requestString);
    foodItem = convertFromProto(protoFoodItem);
    socket.send(response);
    received = true;
  }
  return received;
}

// Convert from C++ struct to Protocol Buffer message
FoodItemProto::FoodItem convertToProto(const FoodItem& foodItem) {
  FoodItemProto::FoodItem protoFoodItem;

  protoFoodItem.set_id(foodItem.getId());
  protoFoodItem.set_image_path(foodItem.getImagePath().string());
  protoFoodItem.set_name(foodItem.getName());
  protoFoodItem.set_category(
      static_cast<FoodItemProto::FoodCategory>(foodItem.getCategory()));
  protoFoodItem.set_weight(foodItem.getWeight());
  protoFoodItem.set_quantity(foodItem.getQuantity());

  // Handle dates
  auto* scan_date = protoFoodItem.mutable_scan_date();
  scan_date->set_year(static_cast<int>(foodItem.getScanDate().year()));
  scan_date->set_month(static_cast<unsigned>(foodItem.getScanDate().month()));
  scan_date->set_day(static_cast<unsigned>(foodItem.getScanDate().day()));

  auto* exp_date = protoFoodItem.mutable_expiration_date();
  exp_date->set_year(static_cast<int>(foodItem.getExpirationDate().year()));
  exp_date->set_month(static_cast<unsigned>(foodItem.getExpirationDate().month()));
  exp_date->set_day(static_cast<unsigned>(foodItem.getExpirationDate().day()));

  return protoFoodItem;
}

// Convert from Protocol Buffer message to C++ struct
FoodItem convertFromProto(const FoodItemProto::FoodItem& protoFoodItem) {
  FoodItem item;

  item.setId(protoFoodItem.id());
  item.setImagePath(protoFoodItem.image_path());
  item.setName(protoFoodItem.name());
  item.setCategory(static_cast<FoodCategories>(protoFoodItem.category()));
  item.setWeight(protoFoodItem.weight());
  item.setQuantity(protoFoodItem.quantity());

  // Handle dates
  item.setScanDate(
      std::chrono::year_month_day{std::chrono::year(protoFoodItem.scan_date().year()),
                                  std::chrono::month(protoFoodItem.scan_date().month()),
                                  std::chrono::day(protoFoodItem.scan_date().day())});

  item.setExpirationDate(std::chrono::year_month_day{
      std::chrono::year(protoFoodItem.expiration_date().year()),
      std::chrono::month(protoFoodItem.expiration_date().month()),
      std::chrono::day(protoFoodItem.expiration_date().day())});

  return item;
}

/**
 * Convert a string to food category
 *
 * @param str String to convert
 * @return The enum version
 */
FoodCategories foodCategoryFromString(const std::string& str) {
  if (str == "unknown")
    return FoodCategories::unknown;
  if (str == "unpackaged")
    return FoodCategories::unpackaged;
  if (str == "packaged")
    return FoodCategories::packaged;
  return FoodCategories::unknown; // Default case
}
