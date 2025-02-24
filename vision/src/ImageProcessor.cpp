#include "../include/ImageProcessor.h"

#include "../include/handleOBJ.h"
#include "../include/handleOCR.h"

/**
 * ImageProcessor Constructor. This class handles all image operations.
 * @param pipes Pipe struct for communication with other processes.
 * @param foodItem FoodItem struct for filling out detected information.
 */
ImageProcessor::ImageProcessor(const Pipes& pipes, FoodItem& foodItem)
    : modelHandler(), pipes(pipes), foodItem(foodItem) {}

/**
 * Parent method to all image processing and analyzing
 */
void ImageProcessor::process() const {
  LOG(INFO) << "Vision analyzing all images";
  if (!isValidDirectory(foodItem.imageDirectory)) {
    LOG(FATAL) << "Failed to open image directory" << foodItem.imageDirectory;
    return;
  }
  bool detectedFoodItem = analyze();
  LOG(INFO) << "Successfully analyzed all images";
  if (!detectedFoodItem) {
    LOG(INFO) << "Item not successfully detected";
    // checked 16 images and failed them all
    //  how to handle?
    // Still send whatever information we do have to the display
  }
  // tell hardware to stop
  LOG(INFO) << "Sent stop signal to hardware";
  bool detectionComplete = true;
  write(pipes.visionToHardware[WRITE], &detectionComplete, sizeof(detectionComplete));

  // send display the food item
  LOG(INFO) << "Sent detected food item to display";
  printFoodItem(foodItem);
  sendFoodItem(foodItem, pipes.visionToDisplay[WRITE]);
}

/**
 * Conduct the analyzing and model processing
 * @return whether FoodItem is successfully identified
 */
bool ImageProcessor::analyze() const {
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
      if (objectDetected) {
        return true;
      }

      if (objectDetected && expirationDateDetected) {
        return true;
      }
    }
    if (imageCounter >= 16) {
      // give up trying to identify
      if (!objectDetected) {
        LOG(INFO) << "Object not able to be classified";
        // tell display
      }
      else if (!expirationDateDetected) {
        LOG(INFO) << "Object expiration date not found";
        // tell display
      }
      return false;
    }
    // check if image directory has new files
    // directory iterator does not update
    while (!hasFiles(foodItem.imageDirectory)) {
      usleep(500000);
    }
  }
  return false;
}

struct FoodItem& ImageProcessor::getFoodItem() { return this->foodItem; }

void ImageProcessor::setFoodItem(struct FoodItem& foodItem) { this->foodItem = foodItem; }