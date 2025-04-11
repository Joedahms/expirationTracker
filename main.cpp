#include <glog/logging.h>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <zmqpp/zmqpp.hpp>

#include "display/src/display_engine.h"
#include "endpoints.h"
#include "hardware/src/hardware_entry.h"
#include "vision/include/visionMain.h"

void checkCommandLineArgs(int argc, char* argv[], bool& usingMotor, bool& usingCamera);

int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);
  zmqpp::context context;

  bool usingMotor  = true;
  bool usingCamera = true;

  checkCommandLineArgs(argc, argv, usingMotor, usingCamera);

  // Display
  LOG(INFO) << "Starting display process..";
  int displayPid;
  if ((displayPid = fork()) == -1) {
    LOG(FATAL) << "Error starting display process";
  }
  else if (displayPid == 0) {
    google::ShutdownGoogleLogging();
    google::InitGoogleLogging("display");

    // Context
    const int windowWidth  = 1024;
    const int windowHeight = 600;
    const bool fullscreen  = false;

    DisplayEngine displayEngine("Display", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                windowWidth, windowHeight, fullscreen, context);
    displayEngine.start();

    LOG(INFO) << "Display process";
    return 0;
  }
  else {
    LOG(INFO) << "Display process started successfully";
  }

  // Hardware
  LOG(INFO) << "Starting hardware process..";
  int hardwarePid;
  if ((hardwarePid = fork()) == -1) {
    LOG(FATAL) << "Error starting hardware process";
  }
  else if (hardwarePid == 0) {
    google::ShutdownGoogleLogging();
    google::InitGoogleLogging("hardware");

    hardwareEntry(context, usingMotor);
    LOG(INFO) << "Hardware process";
    return 0;
  }
  else {
    LOG(INFO) << "Hardware process started successfully";
  }

  // Vision
  LOG(INFO) << "Starting vision process..";
  int visionPid;
  if ((visionPid = fork()) == -1) {
    LOG(FATAL) << "Error starting vision process";
  }
  else if (visionPid == 0) {
    google::ShutdownGoogleLogging();
    google::InitGoogleLogging("vision");

    visionEntry(context);
    LOG(INFO) << "Vision process";
    return 0;
  }
  else {
    LOG(INFO) << "Vision process started successfully";
  }

  int status;
  waitpid(displayPid, &status, 0);
  LOG(INFO) << "Display process terminated with status: " << status;

  waitpid(hardwarePid, &status, 0);
  LOG(INFO) << "Hardware process terminated with status: " << status;

  waitpid(visionPid, &status, 0);
  LOG(INFO) << "Vision process terminated with status: " << status;
  google::ShutdownGoogleLogging();
  return 0;
}

void checkCommandLineArgs(int argc, char* argv[], bool& usingMotor, bool& usingCamera) {
  const std::string noMotorFlag  = "-nomo";
  const std::string noCameraFlag = "-nocam";

  for (int i = 1; i < argc; i++) {
    std::string arg(argv[i]);
    if (arg == noMotorFlag) {
      usingMotor = false;
    }
    else if (arg == noCameraFlag) {
      usingCamera = false;
    }
  }

  std::cout << "Starting expiration tracker..." << std::endl;
  std::cout << "Using motor: " << usingMotor << std::endl;
  std::cout << "Using camera: " << usingCamera << std::endl;
}
