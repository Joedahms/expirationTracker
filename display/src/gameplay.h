#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <SDL2/SDL.h>
#include <memory>
#include <string>

#include "game_global.h"

/**
 * Gameplay state. State entered when user presses start button in main menu.
 */
class Gameplay {
public:
  Gameplay(struct GameGlobal);
  int handleEvents(bool*);
  int checkKeystates();
  void update();
  void render();
  void enterGameplay();
  void initializeTextures();
  bool getStateEntered(); // Check if the state has already been entered

private:
  struct GameGlobal gameGlobal;
  bool stateEntered = false; // Has the state been entered before

  SDL_Texture* selectedTexture;
};

#endif
