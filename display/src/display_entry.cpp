#include <glog/logging.h>
#include <iostream>

#include "../../pipes.h"
#include "display_entry.h"
#include "display_handler.h"
#include "engine/engine_entry.h"
#include "sql_food.h"

/**
 * Entry into the display code. Only called from main after display child process is
 * forked.
 *
 * @param externalPipes Pipes for display to communicate with the other external processes
 * (vision and hardware).
 * @return None
 */
void displayEntry(struct Pipes externalPipes, struct Endpoints endpoints) {
  LOG(INFO) << "Entered display process";

  // Close write end of read pipes
  close(externalPipes.hardwareToDisplay[WRITE]);
  close(externalPipes.visionToDisplay[WRITE]);

  // Close read end of write pipes
  close(externalPipes.displayToHardware[READ]);
  close(externalPipes.displayToVision[READ]);

  // SDL pipes
  int engineToDisplay[2];
  int displayToEngine[2];
  pipe(engineToDisplay);
  pipe(displayToEngine);

  int sdlPid;
  if ((sdlPid = fork()) == -1) {
    LOG(FATAL) << "Error starting SDL process";
  }
  else if (sdlPid == 0) {
    google::ShutdownGoogleLogging();
    google::InitGoogleLogging("display_engine");

    // In engine
    close(engineToDisplay[READ]);
    close(displayToEngine[WRITE]);

    engineEntry(engineToDisplay, displayToEngine);
  }
  else {
    // Still in display entry
    close(engineToDisplay[WRITE]);
    close(displayToEngine[READ]);
    DisplayHandler displayHandler(externalPipes, displayToEngine, engineToDisplay);

    while (1) {
      bool stringFromSdl = false;
      displayHandler.handleExternal();
      displayHandler.handleEngine();
    }
  }
}
