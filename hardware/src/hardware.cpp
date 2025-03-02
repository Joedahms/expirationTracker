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

void Hardware::startScan() {
  LOG(INFO) << "Checking weight";
  /**
   * Function call to scale
   * 0 - nothing on scale
   * 1 - valid input, begin scanning
   */

  LOG(INFO) << "Beginning scan";
  /**
   * Function call to controls routine
   * has a pipe read from vision in loop
   */

  LOG(INFO) << "Sending Images from Hardware to Vision";
  const std::chrono::time_point now{std::chrono::system_clock::now()};

  std::filesystem::path filePath       = "../images/temp";
  std::chrono::year_month_day scanDate = std::chrono::floor<std::chrono::days>(now);
  // TODO
  float weight = 10.0;

  FoodItem foodItem(filePath, scanDate, weight);

  this->requestVisionSocket.connect(this->externalEndpoints.visionEndpoint);
  sendFoodItem(this->requestVisionSocket, foodItem);
  LOG(INFO) << "Done Sending Images from Hardware to Vision";
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
