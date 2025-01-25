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

  // Close unused ends of the pipes
  close(pipes.fromDisplay[WRITE]);
  close(pipes.fromVision[WRITE]);
  close(pipes.toDisplay[READ]);
  close(pipes.toVision[READ]);

  // Close not currently used ends
  close(pipes.fromDisplay[READ]); // Not currently used
  close(pipes.fromVision[READ]);  // Not currently used
  close(pipes.toDisplay[WRITE]);  // Not currently used
  // close(pipes.toVision[WRITE]);   // Not currently used

  LOG(INFO) << "Sending Image from Hardware to Vision";
  FILE* appleImage = fopen("../banana.jpg", "rb");
  if (!appleImage) {
    LOG(FATAL) << "Failed to open JPEG file";
    return;
  }

  // Read the file and write to the pipe
  char bufferImage[4096]; // Buffer for reading file chunks
  ssize_t bytesRead;
  while ((bytesRead = fread(bufferImage, 1, sizeof(bufferImage), appleImage)) > 0) {
    if (write(pipes.toVision[WRITE], bufferImage, bytesRead) == -1) {
      LOG(FATAL) << "Failed to write JPEG data to pipe";
    }
  }

  fclose(appleImage);
  LOG(INFO) << "JPEG file sent to Display process";
}
