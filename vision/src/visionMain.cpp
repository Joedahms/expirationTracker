#include <filesystem>
#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <memory>

#include "../../food_item.h"
#include "../include/ImageProcessor.h"
#include "../include/analyzeImages.h"
#include "../include/helperFunctions.h"
#include "../include/visionMain.h"

/**
 * Entry into the vision code. Only called from main after vision child process is
 * forked.
 *
 * Input:
 * @param pipes Pipes for vision to communicate with the other processes
 * Output: None
 */
void visionEntry(struct Pipes pipes) {
  LOG(INFO) << "Within vision process";
  closeUnusedPipes(pipes);
  struct FoodItem foodItem;
  ImageProcessor processor = ImageProcessor(pipes, foodItem);
  // Wait for start signal from Display with 0.5sec sleep
  while (1) {
    LOG(INFO) << "Waiting for start signal from Hardware";
    if (receiveFoodItem(foodItem, pipes.hardwareToVision[READ], (struct timeval){1, 0})) {
      LOG(INFO) << "Vision Received all images from hardware";
      processor.setFoodItem(foodItem);
      processor.process();
    }
    else {
      usleep(500000);
    }
  }
}
