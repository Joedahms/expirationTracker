#include <glog/logging.h>
#include <iostream>

#include "display/src/display_pipe.h"
#include "hardware/src/hardware_pipe.h"
#include "pipes.h"
#include "vision/src/vision_pipe.h"
#include <atomic>
#include <glog/logging.h>
#include <iostream>

int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);

  DisplayPipes displayPipes;
  VisionPipes visionPipes;
  HardwarePipes hardwarePipes;

  // Initialize all pipes
  initializePipes(displayPipes, visionPipes, hardwarePipes);

  LOG(INFO) << "Starting display process..";
  int displayPid;
  if ((displayPid = fork()) == -1) {
    LOG(FATAL) << "Error starting display process";
  }
  else if (displayPid == 0) {
    displayEntry(displayPipes);
    LOG(INFO) << "Display process";
    return 0;
  }
  else {
    LOG(INFO) << "Display process started successfully";
  }

  LOG(INFO) << "Starting hardware process..";
  int hardwarePid;
  if ((hardwarePid = fork()) == -1) {
    LOG(FATAL) << "Error starting hardware process";
  }
  else if (hardwarePid == 0) {
    hardwareEntry(hardwarePipes);
    LOG(INFO) << "Hardware process";
    return 0;
  }
  else {
    LOG(INFO) << "Hardware process started successfully";
  }

  LOG(INFO) << "Starting vision process..";
  int visionPid;
  if ((visionPid = fork()) == -1) {
    LOG(FATAL) << "Error starting vision process";
  }
  else if (visionPid == 0) {
    visionEntry(visionPipes);
    LOG(INFO) << "Vision process";
    return 0;
  }
  else {
    LOG(INFO) << "Vision process started successfully";
  }

  google::ShutdownGoogleLogging();
  return 0;
}
