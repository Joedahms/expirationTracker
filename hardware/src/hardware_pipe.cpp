#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <string>

#include "hardware_pipe.h"

/**
 * Entry into the hardware code. Only called from main after hardware child process is
 * forked.
 *
 * Input:
 * - Pipes for hardware to communicate with the other processes
 * Output: None
 */
void hardwareEntry(struct HardwarePipes pipes) {
  LOG(INFO) << "Within vision process";

  // not ever used
  close(pipes.fromDisplay[WRITE]);
  close(pipes.fromVision[WRITE]);
  close(pipes.toDisplay[READ]);
  close(pipes.toVision[READ]);

  // not currently used
  close(pipes.fromVision[READ]);
  close(pipes.fromDisplay[READ]);
  close(pipes.toVision[WRITE]);

  LOG(INFO) << "Sending Message from Hardware to Display";

  std::string message = "Hello from Hardware!";
  write(pipes.toDisplay[WRITE], message.c_str(), message.length() + 1);
}
