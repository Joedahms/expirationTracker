#include <filesystem>
#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <string>
#include <unistd.h>

#include "../../food_item.h"
#include "./controls/controls.h"
#include "./controls/weight.h"
#include "io.h"

/**
 * Entry into the hardware code. Only called from main after hardware child process is
 * forked.
 *
 * Input:
 * @param pipes Pipes for hardware to communicate with the other processes
 * @return None
 */
void hardwareEntry(struct Pipes pipes) {
  LOG(INFO) << "Within vision process";

  // Close write end of read pipes
  close(pipes.displayToHardware[WRITE]);
  close(pipes.visionToHardware[WRITE]);

  // Close read end of write pipes
  close(pipes.hardwareToDisplay[READ]);
  close(pipes.hardwareToVision[READ]);

  while (1) {
    // Wait for start signal from Display with 0.5sec sleep
    LOG(INFO) << "Waiting for start signal from Display";
    if (receivedStartSignal(pipes.displayToHardware[READ])) {
      LOG(INFO) << "Checking if there is weight on the platform";
      /**
       * Check if there is weight on the platform
       *
       *
       * @return boolean
       * 0 - nothing on scale
       * 1 - valid input, begin scanning
       */
      if (getWeight() > 0) {
        LOG(INFO) << "Weight detected on platform. Beginning scan.";
        // send 1 to display to indicate weight
        rotateAndCapture();
      }
      else {
        LOG(INFO) << "No weight detected on platform.";
        // send 0 to display to indicate no weight
        continue;
      }
    }
    else {
      usleep(500000);
    }
  }
}