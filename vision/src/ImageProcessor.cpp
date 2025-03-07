#include "../include/ImageProcessor.h"

/**
 * @param context The zeroMQ context with which to creates with
 * @param externalEndpoints Endpoints to the main components of the system (vision,
 * hardware, and display)
 */
ImageProcessor::ImageProcessor(zmqpp::context& context)
    : logger("image_processor.txt"),
      requestHardwareSocket(context, zmqpp::socket_type::request),
      requestDisplaySocket(context, zmqpp::socket_type::request),
      replySocket(context, zmqpp::socket_type::reply), modelHandler(context) {
  try {
    this->requestHardwareSocket.connect(ExternalEndpoints::hardwareEndpoint);
    this->requestDisplaySocket.connect(ExternalEndpoints::displayEndpoint);
    this->replySocket.bind(ExternalEndpoints::visionEndpoint);
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
  this->logger.log("Entering process");

  this->logger.log("Opening directory: " + foodItem.getImagePath().string());
  if (!isValidDirectory(foodItem.getImagePath())) {
    LOG(FATAL) << "Failed to open image directory";
    return;
  }

  bool detectedFoodItem = analyze();
  this->logger.log("Successfully analyzed all images");

  // Move this into analyze
  if (detectedFoodItem) {
    detectionSucceeded();
  }
  else {
    detectionFailed();
  }
}

/**
 * Conduct the analyzing and model processing.
 *
 * @param None
 * @return Whether FoodItem is successfully identified
 */
bool ImageProcessor::analyze() {
  this->logger.log("Entering analyze");
  int imageCounter    = 0;
  bool objectDetected = false;

  while (!objectDetected) {
    for (const auto& entry :
         std::filesystem::directory_iterator(foodItem.getImagePath())) {
      if (toLowerCase(entry.path().extension()) != ".jpg") {
        continue;
      }
      this->logger.log("Analyzing path: " + entry.path().string());
      if (++imageCounter > this->MAX_IMAGE_COUNT) {
        if (objectDetected) {
          this->logger.log("Object expiration date not found");
        }
        else {
          this->logger.log("Object not able to be classified");
        }
        return false;
      }
      continue;
      // Only runs text atm
      if (!objectDetected) {
        if (this->modelHandler.classifyObject(entry.path(), this->foodItem)) {
          objectDetected = true;
          return true;
        }
      }
    }
  }
  return false;
}

struct FoodItem& ImageProcessor::getFoodItem() { return this->foodItem; }
void ImageProcessor::setFoodItem(struct FoodItem& foodItem) { this->foodItem = foodItem; }

void ImageProcessor::detectionSucceeded() {
  this->logger.log("Item successfully detected");
  stopHardware();
  foodItemToDisplay();
}

void ImageProcessor::detectionFailed() {
  this->logger.log("Item not successfully detected");

  this->logger.log("Sending detection failed to display");
  this->requestDisplaySocket.send(Messages::ITEM_DETECTION_FAILED);
  std::string response;
  this->requestDisplaySocket.receive(response);

  if (response == Messages::AFFIRMATIVE) {
    this->logger.log("Display received failure message");
  }
  else {
    LOG(FATAL) << "Received invalid message from display";
  }
}

void ImageProcessor::foodItemToDisplay() {
  this->logger.log("Indicating detection success to display");
  this->requestDisplaySocket.send(Messages::ITEM_DETECTION_SUCCEEDED);
  std::string response;
  this->requestDisplaySocket.receive(response);
  if (response == Messages::AFFIRMATIVE) {
    this->logger.log("Success indicated to display, sending detected food item");
    response = sendFoodItem(this->requestDisplaySocket, this->foodItem);
    if (response == Messages::AFFIRMATIVE) {
      this->logger.log("Detected food item sent successfully");
    }
    else {
      LOG(FATAL) << "Detected food item transmission failure";
    }
  }
}

void ImageProcessor::stopHardware() {
  bool hardwareStopping = false;
  while (hardwareStopping == false) {
    this->logger.log("Sending stop signal hardware");
    this->requestHardwareSocket.send(Messages::ITEM_DETECTION_SUCCEEDED);
    this->logger.log("Sent stop signal to hardware");

    std::string response;
    this->requestHardwareSocket.receive(response);

    if (response == Messages::AFFIRMATIVE) {
      this->logger.log("Hardware received stop signal");
      hardwareStopping = true;
    }
    else if (response == Messages::RETRANSMIT) {
      this->logger.log("Hardware requested retransmission");
    }
    else {
      this->logger.log("Received invalid message from hardware");
      LOG(FATAL) << "Received invalid message from hardware";
    }
  }
}
