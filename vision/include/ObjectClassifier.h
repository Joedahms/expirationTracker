#ifndef OBJECT_CLASSIFICATION_H
#define OBJECT_CLASSIFICATION_H

#include "../../food_item.h"
#include "IModel.h"
#include <filesystem>
#include <glog/logging.h>

class ObjectClassifier : public IModel {
public:
  explicit ObjectClassifier(FoodItem& foodItem) : IModel(foodItem) {}
  std::string runModel(const std::filesystem::path& imagePath) const override;
  bool handleClassification(const std::filesystem::path& imagePath) override;
};

#endif