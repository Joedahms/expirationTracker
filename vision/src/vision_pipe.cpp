#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <string>

#include "vision_pipe.h"

/**
 * Entry into the vision code. Only called from main after vision child process is
 * forked.
 *
 * Input:
 * - Pipes for vision to communicate with the other processes
 * Output: None
 */
void visionEntry(struct VisionPipes pipes) {
  LOG(INFO) << "Within vision process";

  // not ever used
  close(pipes.fromDisplay[WRITE]);
  close(pipes.fromHardware[WRITE]);
  close(pipes.toDisplay[READ]);
  close(pipes.toHardware[READ]);

  // not currently used
  close(pipes.fromHardware[READ]);
  close(pipes.fromDisplay[READ]);
  close(pipes.toHardware[WRITE]);

  LOG(INFO) << "Sending Message from Vision to Display";

  std::string message = "Hello from Vision!";
  write(pipes.toDisplay[WRITE], message.c_str(), message.length() + 1);
}
