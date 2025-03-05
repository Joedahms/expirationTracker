#include "../include/ImageProcessor.h"

/**
 * ImageProcessor Constructor. This class handles all image operations.
 * @param pipes Pipe struct for communication with other processes.
 * @param foodItem FoodItem struct for filling out detected information.
 */
ImageProcessor::ImageProcessor(zmqpp::context& context,
                               const struct ExternalEndpoints& externalEndpoints)
    : externalEndpoints(externalEndpoints),
      requestHardwareSocket(context, zmqpp::socket_type::request),
      requestDisplaySocket(context, zmqpp::socket_type::request),
      replySocket(context, zmqpp::socket_type::reply), modelHandler(context),
      logger("image_processor.txt") {
  try {
    this->requestHardwareSocket.connect(this->externalEndpoints.hardwareEndpoint);
    this->requestDisplaySocket.connect(this->externalEndpoints.displayEndpoint);
    this->replySocket.bind(this->externalEndpoints.visionEndpoint);
  } catch (const zmqpp::exception& e) {
    std::cerr << e.what();
  }
}

/**
 * Parent method to all image processing and analyzing
 */
void ImageProcessor::process() {
  this->logger.log("Vision analyzing all images");

  if (!isValidDirectory(foodItem.getImagePath())) {
    std::cerr << "Failed to open image directory" << foodItem.getImagePath();
    return;
  }

  bool detectedFoodItem = analyze();
  this->logger.log("Successfully analyzed all images");
  if (!detectedFoodItem) {
    this->logger.log("Item not successfully detected");
    // checked 16 images and failed them all
    //  how to handle?
    // Still send whatever information we do have to the display
    //    writeString(pipes.visionToDisplay[WRITE],
    //               "Food item not properly identified. Sending incomplete food item.");
  }

  // tell hardware to stop
  bool detectionComplete = true;
  this->requestHardwareSocket.send("item identified");
  this->logger.log("Sent stop signal to hardware");

  // send display the food item
  // TODO  printFoodItem(foodItem);
  sendFoodItem(this->requestDisplaySocket, this->foodItem);
  this->logger.log("Sent detected food item to display");
}

/**
 * Conduct the analyzing and model processing
 * @return whether FoodItem is successfully identified
 */
bool ImageProcessor::analyze() {
  int imageCounter    = 0;
  bool objectDetected = false;

  while (!objectDetected) {
    for (const auto& entry :
         std::filesystem::directory_iterator(foodItem.getImagePath())) {
      if (toLowerCase(entry.path().extension()) != ".jpg") {
        continue;
      }

      if (++imageCounter > this->MAX_IMAGE_COUNT) {
        if (objectDetected) {
          this->logger.log("Object expiration date not found");
        }
        else {
          this->logger.log("Object not able to be classified");
        }
        return false;
      }

      // check if the object exists in our object classification
      if (!objectDetected) {
        // if object has already been detected no need to run this (still looking for exp
        // date)
        if (this->modelHandler.classifyObject(entry.path())) {
          objectDetected = true;
          return true;
        }
      }

      // delete bad image
      try {
        std::filesystem::remove(entry.path());
        this->logger.log("Deleted unclassified image: " + entry.path().string());
      } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Failed to delete image: " << entry.path() << " - " << e.what();
      }

      // check if image directory has new files
      // directory iterator does not update
      // Wait if the directory is empty
      while (!hasFiles(foodItem.getImagePath())) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
      }
    }
  }
  return false;
}

/**
 * Return the food item
 * @return whether FoodItem is successfully identified
 */
struct FoodItem& ImageProcessor::getFoodItem() { return this->foodItem; }

/**
 * set the food item entirely
 */
void ImageProcessor::setFoodItem(struct FoodItem& foodItem) { this->foodItem = foodItem; }