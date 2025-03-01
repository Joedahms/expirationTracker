#ifndef IMODEL_H
#define IMODEL_H

#include <filesystem>
#include <fstream>
#include <glog/logging.h>
#include <string>

#include "../../food_item.h"
#include "helperFunctions.h"

#define PIPE_IN  "/tmp/image_pipe"
#define PIPE_OUT "/tmp/result_pipe"

enum TaskType { unknown, CLS, OCR, EXP };

class IModel {
protected:
  FoodItem& foodItem;
  std::string readResponse() const;
  void sendRequest(const TaskType&, const std::filesystem::path&) const;
  std::string taskTypeToString(const TaskType&) const;
  virtual std::string runModel(const std::filesystem::path&) const = 0;
  virtual bool handleClassification(const std::filesystem::path&)  = 0;

public:
  explicit IModel(FoodItem& foodItem) : foodItem(foodItem) {}
  virtual ~IModel() = default;
};

#endif // IMODEL_H
