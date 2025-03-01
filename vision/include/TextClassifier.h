#ifndef TEXT_CLASSIFICATION_H
#define TEXT_CLASSIFICATION_H

#include <filesystem>
#include <glog/logging.h>

#include "../../food_item.h"
#include "IModel.h"

class TextClassifier : public IModel {
public:
  explicit TextClassifier(FoodItem& foodItem) : IModel(foodItem) {}
  std::string runModel(const std::filesystem::path& imagePath) const override;
  bool handleClassification(const std::filesystem::path& imagePath) override;
};

#endif
