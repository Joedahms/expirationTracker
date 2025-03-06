#ifndef MODEL_HANDLER_H
#define MODEL_HANDLER_H

#include <filesystem>
#include <glog/logging.h>

#include "../../endpoints.h"
#include "../../food_item.h"
#include "../../logger.h"
#include "ObjectClassifier.h"
#include "TextClassifier.h"

class ModelHandler {
public:
  ModelHandler(zmqpp::context&, const ExternalEndpoints&);
  bool classifyObject(const std::filesystem::path&, FoodItem&);
  bool extractExpirationDate(const std::filesystem::path&) const;

private:
  Logger logger;

  TextClassifier textClassifier;
};

#endif
