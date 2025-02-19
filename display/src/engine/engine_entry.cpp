#include <SDL2/SDL.h>
#include <glog/logging.h>

#include "display_engine.h"
#include "display_global.h"
#include "engine_entry.h"

/**
 * Function called when in the new SDL process. Kicks off the SDL "display" loop.
 *
 * @param None
 * @return None
 */
void engineEntry(int* engineToDisplay, int* displayToEngine) {
  LOG(INFO) << "SDL display process started successfully";

  struct DisplayGlobal displayGlobal;

  // Initialize the displayEngine
  DisplayEngine displayEngine("Display", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              1024, 600, false, displayGlobal);

  while (displayEngine.running()) {
    displayEngine.handleEvents(engineToDisplay, displayToEngine);
    displayEngine.checkState();
    displayEngine.checkKeystates();
    displayEngine.checkState();
    displayEngine.update();
    displayEngine.renderState();
  }
  displayEngine.clean();
}
