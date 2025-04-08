#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#include <glog/logging.h>
#include <thread>

#include "../../endpoints.h"
#include "../../food_item.h"
#include "../../logger.h"
#include "ModelHandler.h"
#include "helperFunctions.h"

enum AnalyzeObjectReturn { Success, Failure, Cancel };

class ImageProcessor {
public:
  explicit ImageProcessor(zmqpp::context&, std::string&);
  void process();
  struct FoodItem& getFoodItem();
  void setFoodItem(struct FoodItem&);
  void requestCancel();
  void resetCancel();
  bool isCancelRequested();
  void notifyServer(const std::string&);

private:
  Logger logger;

  std::atomic_bool cancelRequested = false;

  zmqpp::socket requestHardwareSocket;
  zmqpp::socket requestDisplaySocket;

  ModelHandler modelHandler;
  FoodItem foodItem;

  const int MAX_IMAGE_COUNT = 16;

  void detectionSucceeded();
  void detectionFailed();
  AnalyzeObjectReturn analyze();
  void processImagePair(int, ClassifyObjectReturn&);

  void foodItemToDisplay();
  void stopHardware();
};

#endif
