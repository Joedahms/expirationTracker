#ifndef MODEL_HANDLER_H
#define MODEL_HANDLER_H

#include "../../food_item.h"
#include "ObjectClassifier.h"
#include "TextClassifier.h"
#include "helperFunctions.h"
#include "validateDetection.h"
#include <filesystem>
#include <glog/logging.h>
#include <iostream>
#include <nlohmann/json.hpp>
class ModelHandler {
private:
  ObjectClassifier objectClassifier;
  TextClassifier textClassifier;

public:
  ModelHandler(FoodItem&);
  bool classifyObject(const std::filesystem::path&) const;
  bool extractExpirationDate(const std::filesystem::path&) const;
};

#endif