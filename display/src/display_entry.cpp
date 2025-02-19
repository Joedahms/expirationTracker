#include <glog/logging.h>
#include <iostream>

#include "../../pipes.h"
#include "display_entry.h"
#include "engine/sdl_entry.h"
#include "external_handler.h"
#include "sql_food.h"

/**
 * Entry into the display code. Only called from main after display child process is
 * forked.
 *
 * @param pipes Pipes for display to communicate with the other processes
 * @return None
 */
void displayEntry(struct Pipes pipes) {
  LOG(INFO) << "Entered display process";

  // Close write end of read pipes
  close(pipes.hardwareToDisplay[WRITE]);
  close(pipes.visionToDisplay[WRITE]);

  // Close read end of write pipes
  close(pipes.displayToHardware[READ]);
  close(pipes.displayToVision[READ]);

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
    // In SDL
    close(sdlToDisplay[READ]);
    close(displayToSdl[WRITE]);

    sdlEntry(sdlToDisplay, displayToSdl);
  }
  else {
    // Still in display entry
    close(sdlToDisplay[WRITE]);
    close(displayToSdl[READ]);
    while (1) {
      bool stringFromSdl = false;
      externalHandler(pipes, displayToSdl);
      stringFromSdl = sdlHandler(pipes, sdlToDisplay, displayToSdl);
    }
  }
}
