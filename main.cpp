#include <glog/logging.h>
#include <iostream>

#include "display/src/pipe.h"
#include "pipes.h"
#include "vision/src/pipe.h"

void initializePipes(DisplayPipes&, VisionPipes&, HardwarePipes&);

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
    // display function
    displayEntry(displayPipes);
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

void initializePipes(DisplayPipes& display,
                     VisionPipes& vision,
                     HardwarePipes& hardware) {
  // Display ↔ Hardware
  pipe(display.toHardware);
  hardware.fromDisplay[READ]  = display.toHardware[READ];
  hardware.fromDisplay[WRITE] = display.toHardware[WRITE];

  pipe(display.fromHardware);
  hardware.toDisplay[READ]  = display.fromHardware[READ];
  hardware.toDisplay[WRITE] = display.fromHardware[WRITE];

  // Display ↔ Vision
  pipe(display.toVision);
  vision.fromDisplay[READ]  = display.toVision[READ];
  vision.fromDisplay[WRITE] = display.toVision[WRITE];

  pipe(display.fromVision);
  vision.toDisplay[READ]  = display.fromVision[READ];
  vision.toDisplay[WRITE] = display.fromVision[WRITE];

  // Vision ↔ Hardware
  pipe(vision.toHardware);
  hardware.fromVision[READ]  = vision.toHardware[READ];
  hardware.fromVision[WRITE] = vision.toHardware[WRITE];

  pipe(vision.fromHardware);
  hardware.toVision[READ]  = vision.fromHardware[READ];
  hardware.toVision[WRITE] = vision.fromHardware[WRITE];
}
