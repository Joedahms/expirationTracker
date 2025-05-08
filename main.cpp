#include <glog/logging.h>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <zmqpp/zmqpp.hpp>

#include "display/src/display_engine.h"
#include "endpoints.h"
#include "hardware/src/hardware.h"

HardwareFlags checkCommandLineArgs(int argc, char* argv[]);

int initDisplay(zmqpp::context& context, Logger& mainLogger);
int initHardware(zmqpp::context& context,
                 Logger& mainLogger,
                 const HardwareFlags& hardwareFlags);

int main(int argc, char* argv[]) {
  Logger mainLogger("main.txt");
  zmqpp::context context;

  HardwareFlags hardwareFlags = checkCommandLineArgs(argc, argv);

  int displayPid = initDisplay(context, mainLogger);
  if (displayPid == 0) {
    return 0;
  }
  int hardwarePid = initHardware(context, mainLogger, hardwareFlags);
  if (hardwarePid == 0) {
    return 0;
  }

  int status;
  waitpid(displayPid, &status, 0);
  mainLogger.log("Display process terminated with status: " + status);

  waitpid(hardwarePid, &status, 0);
  mainLogger.log("Hardware process terminated with status: " + status);

  return 0;
}

/**
 * Check passed command line arguments to see if there are any valid ones.
 *
 * @param argc Number of command line arguments.
 * @param argv The command line arguments passed.
 * @return Hardware Flags
 */
HardwareFlags checkCommandLineArgs(int argc, char* argv[]) {
  const std::string noMotorFlag  = "-nomo";
  const std::string noCameraFlag = "-nocam";

  HardwareFlags hardwareFlags;

  for (int i = 1; i < argc; i++) {
    std::string arg(argv[i]);
    if (arg == noMotorFlag) {
      hardwareFlags.usingMotor = false;
    }
    else if (arg == noCameraFlag) {
      hardwareFlags.usingCamera = false;
    }
  }

  std::cout << "Starting expiration tracker..." << std::endl;
  std::cout << "Using motor: " << hardwareFlags.usingMotor << std::endl;
  std::cout << "Using camera: " << hardwareFlags.usingCamera << std::endl;

  return hardwareFlags;
}

/**
 * Initialize the display process.
 *
 * @param context zeroMQ context for creating zeroMQ sockets.
 * @param mainLogger Logger for writing to main's log file.
 * @return The PID of display process forked off from main. Is zero in return from child
 * process.
 */
int initDisplay(zmqpp::context& context, Logger& mainLogger) {
  mainLogger.log("Starting display process..");

  int displayPid;
  if ((displayPid = fork()) == -1) {
    mainLogger.log("Error starting display process");
    LOG(FATAL) << "Error starting display process";
  }
  else if (displayPid == 0) {
    google::InitGoogleLogging("display");

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
    mainLogger.log("Display process started successfully");
    return displayPid;
  }
}

/**
 * Initialize the hardware process.
 *
 * @param context zeroMQ context for creating zeroMQ sockets.
 * @param mainLogger Logger for writing to main's log file.
 * @param usingMotor Flag indicating whether or not to use the DC motor when performing a
 * scan.
 * @param usingCamera Flag indicating whether to use the camera(s) when performing a scan.
 * @return The PID of hardware process forked off from main. Is zero in return from child
 * process.
 */
int initHardware(zmqpp::context& context,
                 Logger& mainLogger,
                 const HardwareFlags& hardwareFlags) {
  mainLogger.log("Starting hardware process..");

  int hardwarePid;
  if ((hardwarePid = fork()) == -1) {
    mainLogger.log("Error starting hardware process");
    LOG(FATAL) << "Error starting hardware process";
  }
  else if (hardwarePid == 0) {
    google::InitGoogleLogging("hardware");

    std::filesystem::path imageDirectory;
    if (hardwareFlags.usingCamera) {
      imageDirectory = std::filesystem::current_path() / "../images/cam/";

      if (!std::filesystem::exists(imageDirectory)) {
        if (std::filesystem::create_directories(imageDirectory)) {
          mainLogger.log("Created directory: " + imageDirectory.string());
        }
        else {
          mainLogger.log("Failed to create directory: " + imageDirectory.string());
        }
      }
    }
    else {
      imageDirectory = std::filesystem::current_path() / "../images/Banana";
    }

    Hardware hardware(context, imageDirectory, hardwareFlags);
    hardware.start();

    LOG(INFO) << "Hardware process";
    return 0;
  }
  else {
    mainLogger.log("Hardware process started successfully");
    return hardwarePid;
  }
}
