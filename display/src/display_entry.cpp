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
void displayEntry(struct Endpoints endpoints) {
  LOG(INFO) << "Entered display process";

  const std::string engineEndpoint = "ipc:///tmp/engine_endpoint";

  int sdlPid;
  if ((sdlPid = fork()) == -1) {
    LOG(FATAL) << "Error starting SDL process";
  }
  else if (sdlPid == 0) {
    google::ShutdownGoogleLogging();
    google::InitGoogleLogging("display_engine");

    engineEntry(engineEndpoint);
  }
  else {
    // Still in display entry
    DisplayHandler displayHandler(endpoints, engineEndpoint);

    while (1) {
      bool stringFromSdl = false;
      displayHandler.handleExternal();
      displayHandler.handleEngine();
    }
  }
}
