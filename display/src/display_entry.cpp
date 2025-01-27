#include <glog/logging.h>
#include <iostream>

#include "../../pipes.h"
#include "display_entry.h"
#include "sdl_entry.h"

/**
 * Entry into the display code. Only called from main after display child process is
 * forked.
 *
 * Input:
 * - Pipes for display to communicate with the other processes
 * Output: None
 */
void displayEntry(struct DisplayPipes pipes) {
  LOG(INFO) << "Entered display process";

  // Close unused ends of the pipes
  close(pipes.toVision[READ]);      // Display does not read from toVision
  close(pipes.toHardware[READ]);    // Display does not read from toHardware
  close(pipes.fromVision[WRITE]);   // Display does not write to fromVision
  close(pipes.fromHardware[WRITE]); // Display does not write to fromHardware

  // SDL pipes
  int sdlToDisplay[2];
  int displayToSdl[2];
  pipe(sdlToDisplay);
  pipe(displayToSdl);

  int sdlPid;
  if ((sdlPid = fork()) == -1) {
    LOG(FATAL) << "Error starting SDL process";
  }
  else if (sdlPid == 0) {
    close(sdlToDisplay[READ]);
    close(displayToSdl[WRITE]);

    sdlEntry();
  }
  else {
    // Still in display entry
    close(sdlToDisplay[WRITE]);
    close(displayToSdl[READ]);
  }

  // Code used to test pipes when first figuring out IPC. Commented out because we know
  // this works but may still need it later for reference.
  /*
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
  */
}
