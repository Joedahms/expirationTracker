#include <glog/logging.h>
#include <iostream>

#include "../../pipes.h"
#include "display_pipe.h"

void displayEntry(struct DisplayPipes pipes) {
  LOG(INFO) << "Within display process";

  // Close unused ends of the pipes
  close(pipes.toVision[READ]);      // Display does not read from toVision
  close(pipes.toHardware[READ]);    // Display does not read from toHardware
  close(pipes.fromVision[WRITE]);   // Display does not write to fromVision
  close(pipes.fromHardware[WRITE]); // Display does not write to fromHardware

  // Close not currently used ends
  close(pipes.fromHardware[READ]); // Not currently used
  close(pipes.toVision[WRITE]);    // Not currently used
  close(pipes.toHardware[WRITE]);  // Not currently used

  LOG(INFO) << "Receiving message from Vision process";
  std::string buffer;
  char character;
  while (read(pipes.fromVision[READ], &character, 1) > 0) {
    if (character != 0) {
      buffer.push_back(character);
    }
  }

  if (buffer.length() == 0) {
    LOG(FATAL) << "Failed to read from Vision process";
  }
  LOG(INFO) << "Display received message: " << buffer;
  std::cout << buffer << std::endl;
}
