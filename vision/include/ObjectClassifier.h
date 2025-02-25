#ifndef OBJECT_CLASSIFICATION_H
#define OBJECT_CLASSIFICATION_H

#include "../../food_item.h"
#include "IModel.h"
#include "validateDetection.h"
#include <filesystem>
#include <glog/logging.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <variant>

class ObjectClassifier : public IModel {
public:
  explicit ObjectClassifier(FoodItem& foodItem) : IModel(foodItem) {}
  std::variant<std::pair<int, float>, std::string> runModel(
      const std::filesystem::path& imagePath) const override;
  bool handleClassification(const std::filesystem::path& imagePath) const override;
};

#endif