#include <filesystem>
#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <memory>

#include "../../food_item.h"
#include "../include/analyzeImages.h"
#include "../include/helperFunctions.h"
#include "../include/vision_pipe.h"

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

  while (1) {
    // Wait for start signal from Display with 0.5sec sleep
    struct FoodItem foodItem;
    LOG(INFO) << "Waiting for start signal from Hardware";
    // foodItemTemplate.imageDirectory is currently the directory of images to look at
    if (receiveFoodItem(foodItem, pipes.hardwareToVision[READ], (struct timeval){1, 0})) {
      LOG(INFO) << "Vision Received all images from hardware";
      processImages(pipes, foodItem);
    }
    else {
      usleep(500000);
    }
  }
}

/**
 * Parent method to all image processing and analyzing
 *
 * Input:
 * @param pipes Pipes for vision to communicate with the other processes
 * @param foodItem food item struct to update data as we collect it
 * Output: None
 */
void processImages(struct Pipes pipes, struct FoodItem& foodItem) {
  LOG(INFO) << "Vision analyzing all images";
  if (!isValidDirectory(foodItem.imageDirectory)) {
    LOG(FATAL) << "Failed to open image directory" << foodItem.imageDirectory;
    return;
  }
  bool detectedFoodItem =
      analyzeImages(foodItem.imageDirectory /= "Strawberry_Package", foodItem);
  LOG(INFO) << "Successfully analyzed all images";
  if (!detectedFoodItem) {
    LOG(FATAL) << "SOMETHING WENT HORRIBLY WRONG";
  }
  // tell hardware to stop
  LOG(INFO) << "Sent stop signal to hardware";
  bool detectionComplete = true;
  write(pipes.visionToHardware[WRITE], &detectionComplete, sizeof(detectionComplete));

  // send display the food item
  LOG(INFO) << "Sent detected food item to display";
  sendFoodItem(foodItem, pipes.visionToDisplay[WRITE]);
}
