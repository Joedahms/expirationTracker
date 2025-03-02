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
  logger.log("Checking for start signal from display");
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
        LOG(INFO) << "Received start signal from display";
        logger.log("Received start signal from display");
        std::cout << "here" << std::endl;
      }
    }
    else {
      LOG(INFO) << "Did not receive start signal from display";
    }
    return receivedRequest;
  } catch (const zmqpp::exception& e) {
    LOG(FATAL) << e.what();
  }
}

bool Hardware::startScan() {
  LOG(INFO) << "Starting scan";
  /**
   * Function call to controls routine
   * has a pipe read from vision in loop
   */

  LOG(INFO) << "Checking weight";
  if (checkWeight() == false) {
    // TODO handle no weight on platform
  }

  rotateAndCapture();
  // TODO
  LOG(INFO) << "Scan completed";
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
  LOG(INFO) << "Sending Images from Hardware to Vision";
  const std::chrono::time_point<std::chrono::system_clock> now{
      std::chrono::system_clock::now()};

  std::filesystem::path filePath       = "../images/temp";
  std::chrono::year_month_day scanDate = std::chrono::floor<std::chrono::days>(now);

  FoodItem foodItem(filePath, scanDate, this->itemWeight);

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
void Hardware::rotateAndCapture() {
  for (int angle = 0; angle < 8; angle++) {
    LOG(INFO) << "At location " << angle << " of 8";

    // TODO
    // This function is for both cameras
    // if (takePhotos(angle) == false) {
    //   LOG(INFO) << "Error taking photos";
    // };

    // Temporary
    if (capturePhoto(angle) == false) {
      LOG(INFO) << "Error taking a photo";
    }

    if (angle == 0) {
      //      sleep(2);
      sendDataToVision();
    }

    LOG(INFO) << "Rotating platform...";
    // TODO rotateMotor();
    usleep(500);

    LOG(INFO) << "Checking for stop signal from vision";
    bool receivedStopSignal = false;
    bool receivedRequest    = false;
    std::string request;
    receivedRequest = this->replySocket.receive(request, true);
    if (receivedRequest) {
      if (request == "item identified") {
        LOG(INFO) << "Received stop signal from vision";
        receivedStopSignal = true;
      }
      else {
        LOG(INFO) << "Received something else from vision";
      }
    }

    if (receivedStopSignal) {
      LOG(INFO) << "AI Vision identified item. Stopping process.";
      break;
    }

    LOG(INFO) << "AI Vision did not identify item. Continuing process.";
  }
}

bool Hardware::capturePhoto(int angle) {
  LOG(INFO) << "Capturing photo at position: " << angle;
  std::string fileName = this->IMAGE_DIRECTORY + std::to_string(angle) + "_test.jpg";

  std::string command = "rpicam-jpeg --output " + fileName;
  system(command.c_str());

  LOG(INFO) << "Photo successfully captured at position: " << angle;

  /*
  pid_t pid;
  if ((pid = fork()) == -1) {
    LOG(FATAL) << "Error starting camera process." << strerror(errno);
    return false;
  }
  else if (pid == 0) {
    google::ShutdownGoogleLogging();
    google::InitGoogleLogging("TakePhotoChild");
    LOG(INFO) << "Capturing photo at position: " << angle;
    std::string command = "rpicam-jpeg --output" + fileName;
    system(command.c_str());
    /*
    execl("/usr/bin/rpicam-jpeg", "rpicam-jpeg", "1920:1080:12:U", "--nopreview",
          "--output", fileName.c_str(), // Save location
          "--timeout", "50", (char*)NULL);
    */
  // LOG(INFO) << "Photo successfully captured at position: " << angle;
  // std::cerr << "Error: Failed to execute rpicam-still" << std::endl;
  // exit(1);
  //}
  // else {
  LOG(INFO) << "Exiting capturePhoto at angle: " << angle;
  return true;
  //}
}
