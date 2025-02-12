#ifndef SCANNING_H
#define SCANNING_H

#include <SDL2/SDL.h>
#include <memory>
#include <string>

#include "../display_global.h"
#include "state.h"

/**
 * Scanning state. State entered when user presses scan new item button in main menu.
 */
class Scanning : public State {
public:
  Scanning(struct DisplayGlobal displayGlobal);
  int handleEvents(bool*) override;
  int checkKeystates();
  void update() override;
  void render() const override;
  void enterScanning();
  void initializeTextures();
  bool getStateEntered(); // Check if the state has already been entered

private:
  bool stateEntered = false; // Has the state been entered before
};

#endif
