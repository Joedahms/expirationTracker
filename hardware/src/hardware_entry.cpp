#include <filesystem>
#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <string>
#include <unistd.h>

#include "../../food_item.h"
#include "./controls/controls.h"
#include "io.h"

/**
 * Entry into the hardware code. Only called from main after hardware child process is
 * forked.
 *
 * Input:
 * @param pipes Pipes for hardware to communicate with the other processes
 * Output: None
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
      redoThis(pipes);
    }
    else {
      usleep(500000);
    }
  }
}