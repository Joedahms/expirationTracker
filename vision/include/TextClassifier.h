#ifndef TEXT_CLASSIFICATION_H
#define TEXT_CLASSIFICATION_H

#include <filesystem>
#include <glog/logging.h>

#include "../../food_item.h"
#include "IModel.h"

class TextClassifier : public IModel {
public:
  TextClassifier(zmqpp::context& context,
                 const std::string& textClassifierEndpoint,
                 const std::string& pythonServerEndpoint);

  std::string runModel(const std::filesystem::path& imagePath) override;
  bool handleClassification(const std::filesystem::path& imagePath) override;

private:
  FoodItem foodItem;
};

#endif
