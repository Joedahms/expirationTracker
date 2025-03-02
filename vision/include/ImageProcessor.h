#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#include <glog/logging.h>
#include <thread>

#include "../../food_item.h"
#include "../../pipes.h"
#include "ModelHandler.h"
#include "helperFunctions.h"

class ImageProcessor {
public:
  explicit ImageProcessor(zmqpp::context& context,
                          const struct ExternalEndpoints& externalEndpoints);
  void process();
  bool analyze();
  struct FoodItem& getFoodItem();
  void setFoodItem(struct FoodItem&);

private:
  ExternalEndpoints externalEndpoints;

  zmqpp::socket requestHardwareSocket;
  zmqpp::socket requestDisplaySocket;
  zmqpp::socket replySocket;

  ModelHandler modelHandler;
  FoodItem foodItem;

  const int MAX_IMAGE_COUNT = 16;
};

#endif
