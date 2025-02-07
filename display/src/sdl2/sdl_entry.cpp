#include <SDL2/SDL.h>
#include <glog/logging.h>

#include "display.h"
#include "display_global.h"
#include "sdl_entry.h"

/**
 * Function called when in the new SDL process. Kicks off the SDL "display" loop.
 *
 * Input: None
 * Output: None
 */
void sdlEntry(int* sdlToDisplay, int* displayToSdl) {
  LOG(INFO) << "SDL display process started successfully";

  struct DisplayGlobal displayGlobal;

  // Initialize the display
  std::unique_ptr<Display> display =
      std::make_unique<Display>("Display", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                1024, 600, false, displayGlobal);

  while (display->running()) {
    display->handleEvents(sdlToDisplay, displayToSdl);
    display->checkState();
    display->checkKeystates();
    display->checkState();
    display->update();
    display->renderState();
  }
  display->clean();
}
