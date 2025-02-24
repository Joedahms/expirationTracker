#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#include "../../food_item.h"
#include "../../pipes.h"
#include "ModelHandler.h"
#include "helperFunctions.h"
#include <glog/logging.h>
#include <iostream>

class ImageProcessor {
private:
  ModelHandler modelHandler;
  const struct Pipes& pipes;
  struct FoodItem& foodItem;

public:
  void process() const;
  bool analyze() const;
  explicit ImageProcessor(const struct Pipes&, FoodItem& foodItem);
  struct FoodItem& getFoodItem();
  void setFoodItem(struct FoodItem&);
};

#endif