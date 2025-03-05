#include <filesystem>
#include <glog/logging.h>

#include "../../food_item.h"
#include "hardware.h"

// Communication setup
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

/*
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

/*
 * Checks for a start signal from the display.
 * Used to start the scan process.ADJ_OFFSET_SINGLESHOT
 *
 * @param None
 * @return bool - True if start signal received, false otherwise
 */
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

/*
 * Function call to controls routine:
 * Checks weight
 * weight = ~0 -> error to Display
 * weight = +int -> start scan
 *
 * @return bool
 * */
bool Hardware::startScan() {
  this->logger.log("Starting scan");

  this->logger.log("Checking weight");
  if (checkWeight() == false) {
    // TODO handle no weight on platform
    // Send error to display
    // Possible pattern HW error msg -> Display message with 3 options:
    // 1. Retry 2. Skip/Override 3. Cancel
    // Response from Display will then decide action
    this->logger.log("No weight detected on platform");
    return false;
  }

  rotateAndCapture();
  this->logger.log("Scan complete");
  return true;
}

// TODO 1. setup zmqpp with Arduino
//      2. integrate code to check weight from Arduino
bool Hardware::checkWeight() {
  this->itemWeight = 1; // set to 1 for testing
  if (this->itemWeight <= 0) {
    // May need to adjust up because the scale is likely sensitive to vibrations
    return false;
  }
  return true;
}

/*
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

    // Leaving in T/F logic in case we need to add error handling later
    if (takePhotos(angle) == false) {
      this->logger.log("Error taking photos");
    };

    // if (capturePhoto(angle) == false) {
    //   this->logger.log("Error taking a photo");
    // }

    if (angle == 0) {
      sendDataToVision();
    }

    this->logger.log("Rotating platform");
    // TODO rotateMotor();

    // Swap comment lines below if you don't want to wait on realistic motor rotation time
    // usleep(500);
    sleep(3);

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
          // Could skip receivedStopSignal and this else statement and just break if "item identified"
          this->logger.log("Received other from vision");
        }
      }
      if (receivedStopSignal) {
        this->logger.log("AI Vision identified item. Stopping process.");
        break;
      }
      this->logger.log("AI Vision did not identify item. Continuing process.");
  }
}

bool Hardware::takePhotos(int angle) {
  this->logger.log("Taking photos at position: " + std::to_string(angle));
  std::string topPhoto  = this->IMAGE_DIRECTORY + std::to_string(angle) + "_top.jpg";
  std::string sidePhoto = this->IMAGE_DIRECTORY + std::to_string(angle) + "_side.jpg";

  std::string command0 = "rpicam-jpeg -0 -n -t 50 1920:1080:10:U --output " + topPhoto;
  std::string command1 = "rpicam-jpeg -1 -n -t 50 1920:1080:10:U --output " + sidePhoto;
  system(command0.c_str());
  system(command1.c_str());

  this->logger.log("Photos successfully captured at position: " + std::to_string(angle));
  this->logger.log("Exiting takePhotos at angle: " + std::to_string(angle));
  // Always returns true
  return true;
}

bool Hardware::capturePhoto(int angle) {
  this->logger.log("Capturing photo at position: " + std::to_string(angle));
  std::string fileName = this->IMAGE_DIRECTORY + std::to_string(angle) + "_test.jpg";

  std::string command = "rpicam-jpeg -n -t 50 1920:1080:12:U --output " + fileName;
  system(command.c_str());

  this->logger.log("Photo successfully captured at position: " + std::to_string(angle));
  this->logger.log("Exiting capturePhoto at angle: " + std::to_string(angle));
  // Always returns true
  return true;
}
