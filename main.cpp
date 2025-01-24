#include <glog/logging.h>
#include <iostream>

#include "display/src/pipe.h"

int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);

  LOG(INFO) << "Starting display process..";
  int displayPid;
  if ((displayPid = fork()) == -1) {
    LOG(FATAL) << "Error starting display process";
  }
  else if (displayPid == 0) {
    // display function
    displayEntry();
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
    // hardware function
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
    // vision function
    LOG(INFO) << "Vision process";
    return 0;
  }
  else {
    LOG(INFO) << "Vision process started successfully";
  }

  return 0;
}
