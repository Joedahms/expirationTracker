#include <SDL2/SDL.h>

#include "game.h"
#include "game_global.h"
#include "sdl_entry.h"

/**
 * Function called when in the new SDL process. Kicks off the SDL "game" loop.
 *
 * Input: None
 * Output: None
 */
void sdlEntry() {
  struct GameGlobal gameGlobal;

  Uint64 frame_start;
  Uint64 frame_end;
  float elapsedMS;

  // Initialize the game
  std::unique_ptr<Game> game =
      std::make_unique<Game>("display", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                             1024, 600, false, gameGlobal);

  while (game->running()) {
    game->handleEvents();
    game->checkState();
    game->checkKeystates();
    game->checkState();
    game->update();
    game->renderState();
  }
  game->clean();
}
