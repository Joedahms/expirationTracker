#ifndef MODEL_HANDLER_H
#define MODEL_HANDLER_H

#include "../../food_item.h"
#include "helperFunctions.h"
#include "validateDetection.h"
#include <filesystem>
#include <glog/logging.h>
#include <iostream>
#include <nlohmann/json.hpp>

class ModelHandler {
private:
public:
  ModelHandler();
  std::pair<int, float> runEfficientNet(const std::filesystem::path&) const;
  bool handleObjectClassification(const std::filesystem::path&, struct FoodItem&) const;
  std::string runClassificationOCR(const std::filesystem::path&) const;
  bool handleClassificationOCR(const std::filesystem::path&, struct FoodItem&) const;
  std::vector<std::string> runExpirationDateOCR(const std::filesystem::path&) const;
  void handleExpirationDateOCR(const std::filesystem::path&, struct FoodItem&) const;
};

#endif