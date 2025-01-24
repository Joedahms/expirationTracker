#include <glog/logging.h>
#include <iostream>

#include "../../pipes.h"
#include "pipe.h"

void displayEntry(struct DisplayPipes pipes) {
  LOG(INFO) << "Within display process";

  // Close unused ends of the pipes
  close(getPipe(pipes.toVision, READ));    // Display does not read from toVision
  close(getPipe(pipes.toHardware, READ)); // Display does not read from toHardware
  close(getPipe(pipes.fromVision, WRITE));  // Display does not write to fromVision
  close(getPipe(pipes.fromHardware, WRITE)); // Display does not write to fromHardware

  // Close not currently used ends
  close(getPipe(pipes.fromHardware, READ)); // Not currently used
  close(getPipe(pipes.toVision, WRITE));    // Not currently used
  close(getPipe(pipes.toHardware, WRITE));  // Not currently used

  LOG(INFO) << "Receiving message from Vision process";
  char buffer[256];
  ssize_t bytesRead = read(getPipe(pipes.fromVision, READ), buffer, sizeof(buffer) - 1);
  if (bytesRead > 0) {
    buffer[bytesRead] = '\0'; // Null-terminate the received string
    LOG(INFO) << "Display received message: " << buffer;
    printf(buffer);
  } else {
    LOG(FATAL) << "Failed to read from Vision process";
  }

  // Close the read end after processing
  close(getPipe(pipes.fromVision, READ));
}