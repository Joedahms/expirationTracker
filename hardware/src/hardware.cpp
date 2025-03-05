#include <filesystem>
#include <glog/logging.h>

#include "../../food_item.h"
#include "hardware.h"

/**
 * @param context The zeroMQ context with which to creates with
 * @param externalEndpoints Endpoints to the main components of the system (vision,
 * hardware, and display)
 */
Hardware::Hardware(zmqpp::context& context,
                   const struct ExternalEndpoints& externalEndpoints)
    : externalEndpoints(externalEndpoints),
      requestVisionSocket(context, zmqpp::socket_type::request),
      requestDisplaySocket(context, zmqpp::socket_type::request),
      replySocket(context, zmqpp::socket_type::reply), logger("hardware_log.txt") {
  try {
    this->requestVisionSocket.connect(this->externalEndpoints.visionEndpoint);
    this->requestDisplaySocket.connect(this->externalEndpoints.displayEndpoint);
    this->replySocket.bind(this->externalEndpoints.hardwareEndpoint);
  } catch (const zmqpp::exception& e) {
    LOG(FATAL) << e.what();
  }
}

/**
 * Check to see if display has sent the signal to initiate the scanning of a new food
 * item.
 *
 * @param None
 * @return True if the start signal was received. False if the signal was not received.
 */
bool Hardware::checkStartSignal(int timeoutMs) {
  bool receivedRequest = false;
  this->logger.log("Checking for start signal from display");

  try {
    zmqpp::poller poller;
    poller.add(this->replySocket);

    if (poller.poll(timeoutMs)) {
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
    LOG(FATAL) << e.what();
  }
}

/**
 * Start the scan of a new food item.
 *
 * @param None
 * @return True if the scan was successful. False if the scan was unsuccessful.
 *
 * TODO Handle no weight on platform
 */
bool Hardware::startScan() {
  this->logger.log("Starting scan");

  this->logger.log("Checking weight");
  if (checkWeight() == false) {
    // TODO handle no weight on platform
  }

  rotateAndCapture();
  this->logger.log("Scan complete");
  return true;
}

/**
 * Read from the weight sensor to get the weight of an object on the platform.
 *
 * @param None
 * @return True if non zero weight. False if zero weight.
 *
 * TODO Read from weight sensor
 */
bool Hardware::checkWeight() {
  this->itemWeight = 1;
  return true;
}

/**
 * Sends the photo directory and weight data to the AI Vision system.
 *
 * @param None
 * @return None
 */
void Hardware::sendDataToVision() {
  this->logger.log("Sending start signal to vision");
  const std::chrono::time_point<std::chrono::system_clock> now{
      std::chrono::system_clock::now()};

  std::filesystem::path filePath       = "../images/temp";
  std::chrono::year_month_day scanDate = std::chrono::floor<std::chrono::days>(now);

  FoodItem foodItem(filePath, scanDate, this->itemWeight);

  this->requestVisionSocket.connect(this->externalEndpoints.visionEndpoint);
  sendFoodItem(this->requestVisionSocket, foodItem);

  this->logger.log("Done sending start signal to vision");
}

/**
 * Rotates platform in 45-degree increments, captures images, and sends the
 * data to the AI Vision system until a full 360-degree rotation is complete.
 * The process can be stopped early if AI Vision sends a "STOP" signal
 * 0 - continue or 1 - stop
 *
 * @param None
 * @return None
 *
 * TODO Multiple cameras
 * TODO Rotate motor
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

    // Initiate scan
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
        this->replySocket.send("got it");
        receivedStopSignal = true;
      }
      else {
        this->logger.log("Received something else from vision");
        this->replySocket.send("retransmit");
      }
    }

    if (receivedStopSignal) {
      this->logger.log("AI Vision identified item. Stopping process.");
      break;
    }

    this->logger.log("AI Vision did not identify item. Continuing process.");
  }
}

/**
 * Capture a single photo using the raspberry pi camera.
 *
 * @param angle Angle the photo was taken at. Used to construct the name of the
 * photo file.
 * @return Always true?
 *
 * TODO Always returns true
 */
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
