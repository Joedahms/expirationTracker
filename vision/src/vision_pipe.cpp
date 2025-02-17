#include <filesystem>
#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <memory>

#include "../../food_item.h"
#include "../include/analyzeImages.h"
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

  // Close write end of read pipes
  close(pipes.displayToVision[WRITE]);
  close(pipes.hardwareToVision[WRITE]);

  // Close read end of write pipes
  close(pipes.visionToDisplay[READ]);
  close(pipes.visionToHardware[READ]);
  while (1) {
    // Wait for start signal from Display with 0.5sec sleep
    LOG(INFO) << "Waiting for start signal from Hardware";
    struct FoodItem foodItemTemplate;
    // foodItemTemplate.photopath is currently the directory of images to look at
    if (receiveFoodItem(foodItemTemplate, pipes.hardwareToVision[READ])) {
      LOG(INFO) << "Vision Received all images from hardware";
      processImages(pipes, foodItemTemplate.photoPath, foodItemTemplate);
    }
    else {
      usleep(500000);
    }
  }
}

void processImages(struct Pipes pipes,
                   std::string imageDirectory,
                   struct FoodItem receivedFoodItem) {
  LOG(INFO) << "Vision analyzing all images";
  FoodItem detectedFoodItem =
      analyzeImages(imageDirectory + "Strawberry_Package", receivedFoodItem);
  if (detectedFoodItem.name == "INVALID PATH") {
    // pathing received was not valid
    // tell hardware
    // return back and wait for another food item to be sent
    return;
  }
  LOG(INFO) << "Successfully analyzed all images";
  std::cout << detectedFoodItem.name << std::endl;

  // tell hardware to stop
  LOG(INFO) << "Sent stop signal to hardware";
  bool detectionComplete = true;
  write(pipes.visionToHardware[WRITE], &detectionComplete, sizeof(detectionComplete));

  // send display the food item
  LOG(INFO) << "Sent detected food item to display";
  sendFoodItem(detectedFoodItem, pipes.visionToDisplay[WRITE]);
}
