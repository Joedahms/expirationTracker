#include "../include/analyzeImages.h"
#include "../../food_item.h"
#include "../include/handleOBJ.h"
#include "../include/handleOCR.h"
#include "../include/validateDetection.h"
#include <filesystem>
#include <glog/logging.h>
#include <iostream>

struct FoodItem analyzeImages(const std::string& imageDirectory,
                              struct FoodItem receivedFoodItem) {
  FoodItem detectedFoodItem;
  if (!std::filesystem::exists(imageDirectory) ||
      !std::filesystem::is_directory(imageDirectory)) {
    LOG(FATAL) << "Failed to open image directory" << imageDirectory;
    detectedFoodItem.name = "INVALID PATH";
    return detectedFoodItem;
  }
  detectedFoodItem.weight = receivedFoodItem.weight;

  bool objectDetected = false;
  while (!objectDetected) {
    for (const auto& entry : std::filesystem::directory_iterator(imageDirectory)) {
      // check if the object exists in our object classification
      auto [detectionIndex, probability] = runEfficientNet(entry.path());
      if (isValidClassification(detectionIndex) && probability > .5) {
        detectedFoodItem.name      = getFoodLabel(detectionIndex);
        detectedFoodItem.category  = "Unpackaged";
        detectedFoodItem.photoPath = entry.path(); // path of photo that classified object
        detectedFoodItem.expirationDate =
            std::chrono::year{2025} / std::chrono::month{2} /
            std::chrono::day{
                20}; // hardcode expiration for now. This will have to be pulled from some
        // sort of database or estimated based on the given class.
        detectedFoodItem.quantity = 1; // default value, in my head this will be updated
                                       // within the display process
        objectDetected = true;
        std::cout << "FOUND" << std::endl;
        return detectedFoodItem;
      }

      // Run text extraction
      auto textDetections = runOCR(entry.path());
      for (const auto& text : textDetections) {
        switch (isValidText(text)) {
        case TextValidationResult::POSSIBLE_CLASSIFICATION:
          objectDetected = true;
          // Handle classification-specific logic here
          break;

        case TextValidationResult::POSSIBLE_EXPIRATION_DATE:
          // Handle expiration-specific logic here
          break;

        case TextValidationResult::NOT_VALID:
          // No action needed for invalid text
          break;
        }
      }

      // Neither classification or text extraction worked on that image
    }

    // check if image directory has new files

    // if not, sleep and wait
  }

  // if not

  return detectedFoodItem;
}