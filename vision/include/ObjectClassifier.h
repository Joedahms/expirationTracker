#ifndef OBJECT_CLASSIFICATION_H
#define OBJECT_CLASSIFICATION_H

#include "../../food_item.h"
#include "../include/helperFunctions.h"
#include "IModel.h"
#include "validateDetection.h"
#include <filesystem>
#include <glog/logging.h>
#include <iostream>
#include <variant>

class ObjectClassifier : public IModel {
public:
  explicit ObjectClassifier(FoodItem& foodItem) : IModel(foodItem) {}
  std::string runModel(const std::filesystem::path& imagePath) const override;
  bool handleClassification(const std::filesystem::path& imagePath) const override;
};

#endif