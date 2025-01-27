#ifndef SCANNING_H
#define SCANNING_H

#include <SDL2/SDL.h>
#include <memory>
#include <string>

#include "../display_global.h"

/**
 * Scanning state. State entered when user presses scan new item button in main menu.
 */
class Scanning {
public:
  Scanning(struct DisplayGlobal);
  int handleEvents(bool*);
  int checkKeystates();
  void update();
  void render();
  void enterScanning();
  void initializeTextures();
  bool getStateEntered(); // Check if the state has already been entered

private:
  struct DisplayGlobal displayGlobal;
  bool stateEntered = false; // Has the state been entered before

  SDL_Texture* selectedTexture;
};

#endif
