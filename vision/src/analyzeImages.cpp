#include "../include/analyzeImages.h"
#include "../../food_item.h"
#include "../include/handleOBJ.h"
#include "../include/handleOCR.h"
#include "../include/helperFunctions.h"
#include <filesystem>
#include <glog/logging.h>
#include <iostream>

/**
 * Parent method to all image processing and analyzing
 *
 * Input:
 * @param pipes Pipes for vision to communicate with the other processes
 * @param foodItem food item struct to update data as we collect it
 * Output: None
 */
bool analyzeImages(struct FoodItem& foodItem) {
  int imageCounter            = 0;
  bool objectDetected         = false;
  bool expirationDateDetected = false;
  while (!objectDetected && !expirationDateDetected) {
    for (const auto& entry :
         std::filesystem::directory_iterator(foodItem.imageDirectory)) {
      imageCounter++;
      // check if the object exists in our object classification
      if (!objectDetected) {
        // if object has already been detected no need to run this
        if (handleObjectClassification(entry.path(), foodItem)) {
          expirationDateDetected = true;
          objectDetected         = true;
          return true;
        }
      }
      // Run text extraction
      handleTextExtraction(entry.path(), foodItem, objectDetected,
                           expirationDateDetected);

      if (objectDetected && expirationDateDetected) {
        return true;
      }
    }
    if (imageCounter >= 16) {
      // give up trying to identify
      return false;
    }
    // check if image directory has new files
    // directory iterator does not update
    while (!hasFiles(foodItem.imageDirectory)) {
      usleep(500000);
    }
  }

  // if not

  return false;
}