#include <filesystem>
#include <glog/logging.h>

#include "../../food_item.h"
#include "hardware.h"

Hardware::Hardware(zmqpp::context& context,
                   const struct ExternalEndpoints& externalEndpoints)
    : externalEndpoints(externalEndpoints),
      requestVisionSocket(context, zmqpp::socket_type::request),
      requestDisplaySocket(context, zmqpp::socket_type::request),
      replySocket(context, zmqpp::socket_type::reply) {}

bool Hardware::checkStartSignal() {
  replySocket.bind(this->externalEndpoints.hardwareEndpoint);

  bool receivedRequest = false;
  std::string request;
  receivedRequest = replySocket.receive(request, true);
  if (receivedRequest) {
    std::cout << request << std::endl;
  }

  replySocket.close();
  return receivedRequest;
}

bool Hardware::startScan() {
  LOG(INFO) << "Checking weight";
  /**
   * Function call to scale
   * 0 - nothing on scale
   * 1 - valid input, begin scanning
   */
  // Logic for checking weight and sending sig to display
  // return false if no weight
  float weight = 1;

  LOG(INFO) << "Beginning scan";
  /**
   * Function call to controls routine
   * has a pipe read from vision in loop
   */

  sendDataToVision(weight);
  return true;
}

/**
 * Sends the photo directory and weight data to the AI Vision system.
 *
 * @param weight The weight of the object on the platform.
 * @return None
 */
void Hardware::sendDataToVision(float weight) {
  LOG(INFO) << "Sending Images from Hardware to Vision";
  const std::chrono::time_point<std::chrono::system_clock> now{
      std::chrono::system_clock::now()};

  /*
  struct FoodItem foodItem;
  foodItem.imageDirectory = std::filesystem::absolute("../images");
  foodItem.scanDate       = std::chrono::floor<std::chrono::days>(now);
  foodItem.weight         = weight;
  sendFoodItem(foodItem, pipeToWrite);
  */

  std::filesystem::path filePath       = "../images/temp";
  std::chrono::year_month_day scanDate = std::chrono::floor<std::chrono::days>(now);
  // TODO

  FoodItem foodItem(filePath, scanDate, weight);

  this->requestVisionSocket.connect(this->externalEndpoints.visionEndpoint);
  sendFoodItem(this->requestVisionSocket, foodItem);

  LOG(INFO) << "Done Sending Images from Hardware to Vision";
}

/**
 * Rotates platform in 45-degree increments, captures images, and sends the
 * data to the AI Vision system until a full 360-degree rotation is complete.
 * The process can be stopped early if AI Vision sends a "STOP" signal
 * 0 - continue or 1 - stop
 *
 * @return None
 */
void Hardware::rotateAndCapture(float weight) {
  for (int angle = 0; angle < 8; angle++) {
    LOG(INFO) << "At location " << angle << " of 8";

    // This function is for both cameras
    // if (takePhotos(angle) == false) {
    //   LOG(INFO) << "Error taking photos";
    // };

    if (capturePhoto(angle) == false) {
      LOG(INFO) << "Error taking a photo";
    }; // This function is for one camera/testing
    if (angle == 0) {
      sleep(2);
      sendDataToVision(weight);
    }

    LOG(INFO) << "Rotating platform...";
    std::cout << "Rotating platform" << std::endl;
    //    rotateMotor();
    usleep(500);

    bool receivedStopSignal = false;
    bool receivedRequest    = false;
    std::string request;
    this->replySocket.bind(this->externalEndpoints.hardwareEndpoint);
    receivedRequest = replySocket.receive(request, true);
    if (receivedRequest) {
      std::cout << request << std::endl;
      // check if stop and set receivedStopSignal accordingly
    }
    replySocket.close();

    if (receivedStopSignal) {
      LOG(INFO) << "AI Vision identified item. Stopping process.";
      std::cout << "AI Vision identified item. Stopping process." << std::endl;
      break;
    }

    LOG(INFO) << "AI Vision did not identify item. Continuing process.";
    std::cout << "Spin again!" << std::endl;
  }
}

bool Hardware::capturePhoto(int angle) {
  std::string fileName = this->IMAGE_DIRECTORY + std::to_string(angle) + "_test.jpg";
  google::ShutdownGoogleLogging();

  pid_t pid = fork();
  if (pid == -1) {
    LOG(FATAL) << "Error starting camera process." << strerror(errno);
    return false;
  }
  if (pid == 0) {
    // Reinitialize glog in the child process
    google::InitGoogleLogging("TakePhotoChild");
    LOG(INFO) << "Capturing at position " << angle;
    execl("/usr/bin/rpicam-jpeg", "rpicam-jpeg", "1920:1080:12:U", "--nopreview",
          "--output", fileName.c_str(), // Save location
          "--timeout", "50", (char*)NULL);

    std::cerr << "Error: Failed to execute rpicam-still" << std::endl;
    exit(1);
  }
  google::InitGoogleLogging("HardwareParent");
  LOG(INFO) << "Exiting photos at " << angle;
  return true;
}
