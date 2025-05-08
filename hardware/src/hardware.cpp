#include <errno.h>
#include <filesystem>
#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <wiringPi.h>

#include "../../zero.h"
#include "hardware.h"
#include "network.h"
#include "wiringSerial.h"

/**
 * @param context The zeroMQ context with which to creates with
 * @param externalEndpoints Endpoints to the main components of the system (vision,
 * hardware, and display)
 */
Hardware::Hardware(zmqpp::context& context, const HardwareFlags& hardwareFlags)
    : logger("hardware_log.txt"), network("../network_config.json"),
      requestServerSocket(context, zmqpp::socket_type::request),
      requestDisplaySocket(context, zmqpp::socket_type::request),
      replySocket(context, zmqpp::socket_type::reply),
      usingMotor(hardwareFlags.usingMotor), usingCamera(hardwareFlags.usingCamera) {
  if (this->usingCamera) {
    this->imageDirectory = std::filesystem::current_path() / "tmp/images/";

    if (!std::filesystem::exists(imageDirectory)) {
      if (std::filesystem::create_directories(imageDirectory)) {
        this->logger.log("Created directory: " + imageDirectory.string());
      }
      else {
        this->logger.log("Failed to create directory: " + imageDirectory.string());
      }
    }
  }
  else {
    this->imageDirectory = std::filesystem::current_path() / "../images/Banana";
  }

  try {
    this->network.connectToServer(this->requestServerSocket, this->logger);
    this->requestDisplaySocket.connect(ExternalEndpoints::displayEndpoint);
    this->replySocket.bind(ExternalEndpoints::hardwareEndpoint);
  } catch (const zmqpp::exception& e) {
    LOG(FATAL) << e.what();
  }
}

void Hardware::start() {
  bool startSignalReceived       = false;
  const int startSignalTimeoutMs = 1000;

  if (this->usingMotor) {
    initDC();
  }

  while (1) {
    startSignalReceived = false;
    while (startSignalReceived == false) {
      startSignalReceived = checkStartSignal(startSignalTimeoutMs);
      if (startSignalReceived == false) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
      }
      else {
      }
    }

    // sendMessage(requestServerSocket, "start", this->logger);
    // sendMessage(requestServerSocket, "stop", this->logger);

    startScan();
  }
}

void Hardware::initDC() {
  this->logger.log("Initializing Motor");

  wiringPiSetupPinType(WPI_PIN_BCM);

  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);

  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);

  this->logger.log("Motor Initialized.");
}

/**
 * Checks for a start signal from the display. Return if have not received a message by
 * the timeout.
 *
 * @param None
 * @return bool - True if start signal received, false otherwise
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
        this->logger.log("Received start signal from display");
        receivedRequest = true;
        this->replySocket.send(Messages::AFFIRMATIVE); // Respond to display
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
 * @param usingCamera True if a raspberry pi camera is connected and can be used to take
 * pictures. False if no camera connected and pictures from images/Apple should be used.
 * @return True if the scan was successful. False if the scan was unsuccessful.
 *
 * TODO Handle no weight on platform
 */
bool Hardware::startScan() {
  this->logger.log("Starting scan");

  if (!this->imageDirectory.empty() && this->usingCamera) {
    this->logger.log("Clearing image directory");
    try {
      for (const auto& entry : std::filesystem::directory_iterator(imageDirectory)) {
        if (entry.is_regular_file()) {
          std::filesystem::remove(entry.path());
          this->logger.log("Deleted image: " + entry.path().string());
        }
      }
    } catch (const std::filesystem::filesystem_error& e) {
      this->logger.log("Error deleting images: " + std::string(e.what()));
    }
  }

  rotateAndCapture();
  this->logger.log("Scan complete");
  return true;
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
    this->logger.log("At location " + std::to_string(angle + 1) + " of 8");

    if (this->usingCamera) {
      takePhotos();
    }

    if (this->usingMotor) {
      rotateMotor(true);
    }
    else {
      this->logger.log("Simulate motor spinning");
      sleep(1);
    }

    // Last iteration doensn't need to check signal
    if (angle == 7) {
      return;
    }

    this->logger.log("Checking for stop signal from vision");
    bool receivedStopSignal = false;
    bool receivedRequest    = false;
    std::string request;
    receivedRequest = this->replySocket.receive(request, true);
    if (receivedRequest) {
      if (request == Messages::ITEM_DETECTION_SUCCEEDED) {
        this->logger.log("Received stop signal from vision");
        this->replySocket.send(Messages::AFFIRMATIVE);
        receivedStopSignal = true;
      }
      else {
        this->logger.log("Received other: " + request);
        this->replySocket.send(Messages::RETRANSMIT);
      }
    }
    if (receivedStopSignal) {
      this->logger.log("AI Vision identified item. Stopping process.");
      digitalWrite(MOTOR_IN1, LOW);
      digitalWrite(MOTOR_IN2, LOW);
      break;
    }
    this->logger.log("AI Vision did not identify item. Continuing process.");
  }
}

/**
 * Takes two photos, one top & side, saves them to the image directory, and logs
 * Note: current "top camera" is ribbon port closer to USB
 *
 * @param int angle - the position of the platform for unique photo ID
 * @return bool - always true
 */
void Hardware::takePhotos() {
  this->logger.log("Taking photos at position: " + std::to_string(angle));
  const std::string cmd0 = "rpicam-jpeg --camera 0";
  const std::string cmd1 = "rpicam-jpeg --camera 1";
  const std::string np   = " --nopreview";
  const std::string res  = " --width 4608 --height 2592";
  const std::string out  = " --output ";

  const std::string topPhotoPath =
      this->imageDirectory.string() + std::to_string(angle) + "_top.jpg";
  const std::string sidePhotoPath =
      this->imageDirectory.string() + std::to_string(angle) + "_side.jpg";

  const std::string command0 = cmd0 + np + res + out + topPhotoPath;
  const std::string command1 = cmd1 + np + res + out + sidePhotoPath;
  system(command0.c_str());
  system(command1.c_str());

  this->logger.log("Photos successfully captured at position: " +
                   std::to_string(this->angle));
}

/**
 * Controls platform rotation with L298N motor driver.
 * Rotates for a fixed duration.
 * Timing will need to be adjusted.
 * HIGH,LOW == forward LOW,HIGH == backwards
 * @param clockwise Boolean; true for clockwise, false for counterclockwise
 *
 * @return None
 */
void Hardware::rotateMotor(bool clockwise) {
  this->logger.log("Rotating platform");
  if (clockwise) {
    this->logger.log("Rotating clockwise.");
    digitalWrite(MOTOR_IN1, HIGH);
    digitalWrite(MOTOR_IN2, LOW);
    usleep(902852);
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, LOW); // HIGH,HIGH || LOW,LOW == off
  }
  else {
    this->logger.log("Rotating counter-clockwise.");
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, HIGH);
    usleep(902852);
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, LOW);
  }
  this->logger.log("Platform successfully rotated");
}
