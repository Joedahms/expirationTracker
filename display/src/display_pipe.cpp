#include <glog/logging.h>
#include <iostream>

#include "../../pipes.h"
#include "display_pipe.h"

/**
 * Entry into the display code. Only called from main after display child process is
 * forked.
 *
 * Input:
 * - Pipes for display to communicate with the other processes
 * Output: None
 */
void displayEntry(struct DisplayPipes pipes) {
  LOG(INFO) << "Within display process";

  // Close unused ends of the pipes
  close(pipes.toVision[READ]);      // Display does not read from toVision
  close(pipes.toHardware[READ]);    // Display does not read from toHardware
  close(pipes.fromVision[WRITE]);   // Display does not write to fromVision
  close(pipes.fromHardware[WRITE]); // Display does not write to fromHardware

  // Close not currently used ends
  close(pipes.toVision[WRITE]);    // Not currently used
  close(pipes.toHardware[WRITE]);  // Not currently used
  close(pipes.fromHardware[READ]); // Not currently used
  close(pipes.fromVision[READ]);   // Not currently used
}
