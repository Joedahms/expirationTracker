#include <SDL2/SDL.h>
#include <glog/logging.h>

#include "display_engine.h"
#include "display_global.h"
#include "sdl_entry.h"

/**
 * Function called when in the new SDL process. Kicks off the SDL "display" loop.
 *
 * @param None
 * @return None
 */
void sdlEntry(int* sdlToDisplay, int* displayToSdl) {
  LOG(INFO) << "SDL display process started successfully";

  struct DisplayGlobal displayGlobal;

  // Initialize the displayEngine
  DisplayEngine displayEngine("Display", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              1024, 600, false, displayGlobal);

  while (displayEngine.running()) {
    displayEngine.handleEvents(sdlToDisplay, displayToSdl);
    displayEngine.checkState();
    displayEngine.checkKeystates();
    displayEngine.checkState();
    displayEngine.update();
    displayEngine.renderState();
  }
  displayEngine.clean();
}
