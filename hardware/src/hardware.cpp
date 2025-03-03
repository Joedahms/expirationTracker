#include <filesystem>
#include <glog/logging.h>

#include "../../food_item.h"
#include "hardware.h"

Hardware::Hardware(zmqpp::context& context,
                   const struct ExternalEndpoints& externalEndpoints)
    : externalEndpoints(externalEndpoints),
      requestVisionSocket(context, zmqpp::socket_type::request),
      requestDisplaySocket(context, zmqpp::socket_type::request),
      replySocket(context, zmqpp::socket_type::reply), logger("hardware_log.txt") {
  // Setup sockets
  try {
    this->requestVisionSocket.connect(this->externalEndpoints.visionEndpoint);
    this->requestDisplaySocket.connect(this->externalEndpoints.displayEndpoint);
    this->replySocket.bind(this->externalEndpoints.hardwareEndpoint);
  } catch (const zmqpp::exception& e) {
    std::cerr << e.what();
  }
}

bool Hardware::checkStartSignal() {
  this->logger.log("Checking for start signal from display");
  bool receivedRequest = false;
  try {
    zmqpp::poller poller;
    poller.add(this->replySocket);

    if (poller.poll(1000)) {
      if (poller.has_input(this->replySocket)) {
        std::string request;
        this->replySocket.receive(request);
        receivedRequest = true;
        this->replySocket.send("got it"); // Respond to display
        this->logger.log("Received start signal from display");
      }
    }
    else {
      this->logger.log("Did not receive start signal from display");
    }
    return receivedRequest;
  } catch (const zmqpp::exception& e) {
    std::cerr << e.what();
  }
}

bool Hardware::startScan() {
  this->logger.log("Starting scan");
  /**
   * Function call to controls routine
   * has a pipe read from vision in loop
   */

  this->logger.log("Checking weight");
  if (checkWeight() == false) {
    // TODO handle no weight on platform
  }

  rotateAndCapture();
  // TODO
  this->logger.log("Scan complete");
  return true;
}

// TODO
bool Hardware::checkWeight() {
  this->itemWeight = 1;
  return true;
}

/**
 * Sends the photo directory and weight data to the AI Vision system.
 *
 * @param weight The weight of the object on the platform.
 * @return None
 */
void Hardware::sendDataToVision() {
  this->logger.log("Sending images from hardware to vision");
  const std::chrono::time_point<std::chrono::system_clock> now{
      std::chrono::system_clock::now()};

  std::filesystem::path filePath       = "../images/temp";
  std::chrono::year_month_day scanDate = std::chrono::floor<std::chrono::days>(now);

  FoodItem foodItem(filePath, scanDate, this->itemWeight);

  this->requestVisionSocket.connect(this->externalEndpoints.visionEndpoint);
  sendFoodItem(this->requestVisionSocket, foodItem);

  this->logger.log("Done sending images from hardware to vision");
}

/**
 * Rotates platform in 45-degree increments, captures images, and sends the
 * data to the AI Vision system until a full 360-degree rotation is complete.
 * The process can be stopped early if AI Vision sends a "STOP" signal
 * 0 - continue or 1 - stop
 *
 * @return None
 */
void Hardware::rotateAndCapture() {
  for (int angle = 0; angle < 8; angle++) {
    this->logger.log("At location " + std::to_string(angle) + " of 8");

    // TODO
    // This function is for both cameras
    // if (takePhotos(angle) == false) {
    //   LOG(INFO) << "Error taking photos";
    // };

    // Temporary
    if (capturePhoto(angle) == false) {
      this->logger.log("Error taking a photo");
    }

    if (angle == 0) {
      sendDataToVision();
    }

    this->logger.log("Rotating platform");
    // TODO rotateMotor();
    usleep(500);

    this->logger.log("Checking for stop signal from vision");
    bool receivedStopSignal = false;
    bool receivedRequest    = false;
    std::string request;
    receivedRequest = this->replySocket.receive(request, true);
    if (receivedRequest) {
      if (request == "item identified") {
        this->logger.log("Received stop signal from vision");
        receivedStopSignal = true;
      }
      else {
        this->logger.log("Received something else from vision");
      }
    }

    if (receivedStopSignal) {
      this->logger.log("AI Vision identified item. Stopping process.");
      break;
    }

    this->logger.log("AI Vision did not identify item. Continuing process.");
  }
}

bool Hardware::capturePhoto(int angle) {
  this->logger.log("Capturing photo at position: " + std::to_string(angle));
  std::string fileName = this->IMAGE_DIRECTORY + std::to_string(angle) + "_test.jpg";

  std::string command = "rpicam-jpeg --output " + fileName + " -n";
  system(command.c_str());

  this->logger.log("Photo successfully captured at position: " + std::to_string(angle));
  this->logger.log("Exiting capturePhoto at angle: " + std::to_string(angle));
  // TODO always returns true
  return true;
}
