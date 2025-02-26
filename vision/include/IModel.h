#ifndef IMODEL_H
#define IMODEL_H

#include "../../food_item.h"
#include <filesystem>
#include <string>
#include <variant>
class IModel {
protected:
  FoodItem& foodItem;

public:
  explicit IModel(FoodItem& foodItem) : foodItem(foodItem) {}
  virtual ~IModel() = default;

  /**
   * Runs the model script.
   * @param imagePath Path to the image.
   * @return Either a pair<int, float> (classification index & probability) or a string
   * (extracted text).
   */
  virtual std::string runModel(const std::filesystem::path& imagePath) const = 0;

  /**
   * Handles classification based on the model's return.
   * @param imagePath Path to the image.
   * @param foodItem Struct to update.
   * @return Whether classification was successful.
   */
  virtual bool handleClassification(const std::filesystem::path& imagePath) const = 0;
};

#endif // IMODELHANDLER_H