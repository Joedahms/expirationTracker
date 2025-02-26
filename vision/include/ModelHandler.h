#ifndef MODEL_HANDLER_H
#define MODEL_HANDLER_H

#include "../../food_item.h"
#include "ObjectClassifier.h"
#include "TextClassifier.h"
#include <filesystem>
#include <glog/logging.h>
class ModelHandler {
private:
  ObjectClassifier objectClassifier;
  TextClassifier textClassifier;

public:
  ModelHandler(FoodItem&);
  bool classifyObject(const std::filesystem::path&);
  bool extractExpirationDate(const std::filesystem::path&) const;
};

#endif