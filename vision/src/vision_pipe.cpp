#include <filesystem>
#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <memory>

#include "../../food_item.h"
#include "FoodClasses.h"
#include "handleOCR.h"
#include "vision_pipe.h"

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
      processFoodItems(pipes, foodItemTemplate.photoPath, foodItemTemplate);
    }
    else {
      usleep(500000);
    }
  }
}

void processFoodItems(struct Pipes pipes,
                      std::string imageDirectory,
                      struct FoodItem detectedFoodItem) {
  LOG(INFO) << "Vision analyzing all images";
  std::cout << "Analyzing..." << std::endl;
  bool detectionComplete = analyzeImages(imageDirectory, detectedFoodItem);
  if (detectionComplete) {
    // send display the food item
    LOG(INFO) << "Vision successfully analyzed all images";
    sendFoodItem(detectedFoodItem, pipes.visionToDisplay[WRITE]);
  }
  else {
    // tell hardware to send more images
    writeString(pipes.visionToHardware[WRITE], "HELPPPPP");
  }
}

bool analyzeImages(const std::string& imageDirectory, struct FoodItem detectedFoodItem) {
  if (!std::filesystem::exists(imageDirectory) ||
      !std::filesystem::is_directory(imageDirectory)) {
    LOG(FATAL) << "Failed to open image directory" << imageDirectory;
    return false;
  }

  // check if the object exists in our object classification
  bool objectDetected = false;
  for (const auto& entry : std::filesystem::directory_iterator(imageDirectory)) {
    auto [detectionIndex, probability] = runEfficientNet(entry.path());
    if (isValidClass(detectionIndex) && probability > .5) {
      detectedFoodItem.name      = getFoodLabel(detectionIndex);
      detectedFoodItem.category  = "Unpackaged";
      detectedFoodItem.photoPath = entry.path(); // path of photo that classified object
      detectedFoodItem.expirationDate =
          std::chrono::year{2025} / std::chrono::month{2} /
          std::chrono::day{
              20}; // hardcode expiration for now. This will have to be pulled from some
      // sort of database or estimated based on the given class.
      detectedFoodItem.quantity =
          1; // default value, in my head this will be updated within the display process
      objectDetected = true;
      std::cout << detectedFoodItem.category << std::endl;
      std::cout << detectedFoodItem.expirationDate << std::endl;
      std::cout << detectedFoodItem.name << std::endl;
      std::cout << detectedFoodItem.photoPath << std::endl;
      std::cout << detectedFoodItem.quantity << std::endl;
      std::cout << detectedFoodItem.scanDate << std::endl;
      std::cout << detectedFoodItem.weight << std::endl;

      return true;
    }
  }
  // if not, extract text
  if (!objectDetected) {
    LOG(INFO) << "Running text extraction script";
    std::string result = runOCR(imageDirectory + "cinnamontoastbox.jpg");
    std::cout << "TEXT" << std::endl;
    std::cout << result << std::endl;

    // if text exists in dictionary
    //  if text is a date of some kind
    return true;
  }

  return false;
}
