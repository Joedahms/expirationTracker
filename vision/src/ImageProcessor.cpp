#include "../include/ImageProcessor.h"

/**
 * @param context The zeroMQ context with which to creates with
 * @param externalEndpoints Endpoints to the main components of the system (vision,
 * hardware, and display)
 */
ImageProcessor::ImageProcessor(zmqpp::context& context,
                               const struct ExternalEndpoints& externalEndpoints)
    : externalEndpoints(externalEndpoints), logger("image_processor.txt"),
      requestHardwareSocket(context, zmqpp::socket_type::request),
      requestDisplaySocket(context, zmqpp::socket_type::request),
      replySocket(context, zmqpp::socket_type::reply),
      modelHandler(context, externalEndpoints) {
  try {
    this->requestHardwareSocket.connect(this->externalEndpoints.hardwareEndpoint);
    this->requestDisplaySocket.connect(this->externalEndpoints.displayEndpoint);
    this->replySocket.bind(this->externalEndpoints.visionEndpoint);
  } catch (const zmqpp::exception& e) {
    LOG(FATAL) << e.what();
  }
}

/**
 * Parent method to all image processing and analyzing. Initializes the process of
 * identifying a food item.
 *
 * @param None
 * @return None
 */
void ImageProcessor::process() {
  this->logger.log("Vision analyzing all images");

  if (!isValidDirectory(foodItem.getImagePath())) {
    std::cerr << "Failed to open image directory" << foodItem.getImagePath();
    return;
  }

  bool detectedFoodItem = analyze();
  this->logger.log("Successfully analyzed all images");

  tellHardwareToStop();

  if (!detectedFoodItem) {
    tellDisplayWeFailed();
    return;
  }

  /*
  // TODO
  // send display the food item
  // TODO  printFoodItem(foodItem);
  sendFoodItem(this->requestDisplaySocket, this->foodItem);
  this->logger.log("Sent detected food item to display");
  */
}

/**
 * Conduct the analyzing and model processing.
 *
 * @param None
 * @return Whether FoodItem is successfully identified
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

      // Only runs text atm
      if (!objectDetected) {
        // TODO classifyObject always returns true
        if (this->modelHandler.classifyObject(entry.path(), this->foodItem)) {
          objectDetected = true;
          return true;
        }
      }

      // delete bad image
      try {
        std::filesystem::remove(entry.path());
        this->logger.log("Deleted unclassified image: " + entry.path().string());
      } catch (const std::filesystem::filesystem_error& e) {
        LOG(FATAL) << "Failed to delete image: " << entry.path() << " - " << e.what();
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
 * Tells hardware to stop taking images. Waits for response.
 */
void ImageProcessor::tellHardwareToStop() {
  // Tell hardware to stop
  bool hardwareStopping = false;
  while (hardwareStopping == false) {
    this->logger.log("Sending stop signal to hardware");
    this->requestHardwareSocket.send("item identified");
    this->logger.log("Sent stop signal to hardware");
    std::string response;
    this->requestHardwareSocket.receive(response);
    if (response != "retransmit") {
      hardwareStopping = true;
      this->logger.log("Hardware received stop signal");
    }
    else {
      this->logger.log("Hardware did not receive stop signal, retrying");
    }
  }
}

/**
 * Tells display we failed to ID object. Waits for response.
 */
void ImageProcessor::tellDisplayWeFailed() {
  this->logger.log("Item not successfully detected");
  // Tell display we failed
  bool notifiedDisplayOfFailure = false;
  std::string response;
  while (!notifiedDisplayOfFailure) {
    this->logger.log("Notifying display of failure");
    this->requestHardwareSocket.send("item identification failed");
    this->logger.log("Send notification to display");
    this->requestHardwareSocket.receive(response);
    if (response != "retransmit") {
      notifiedDisplayOfFailure = true;
      this->logger.log("Display received notification");
    }
    else {
      this->logger.log("Display did not receive notification, retrying");
    }
  }
}

/**
 * Return the food item
 * @return whether FoodItem is successfully identified
 */
FoodItem& ImageProcessor::getFoodItem() { return this->foodItem; }

/**
 * set the food item entirely
 */
void ImageProcessor::setFoodItem(struct FoodItem& foodItem) { this->foodItem = foodItem; }