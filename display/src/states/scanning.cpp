#include <SDL2/SDL.h>
#include <assert.h>
#include <glog/logging.h>
#include <iostream>
#include <string>

#include "../display_global.h"
#include "scanning.h"

/**
 * Input:
 * @param displayGlobal Global variables
 */
Scanning::Scanning(struct DisplayGlobal displayGlobal) {
  this->displayGlobal = displayGlobal;
}

/**
 * Handle all events in the SDL event queue.
 *
 * Input:
 * @param displayIsRunning Whether or not the display is running
 * @return The current state of the display after updating scanning
 */
int Scanning::handleEvents(bool* displayIsRunning) {
  SDL_Event event;
  while (SDL_PollEvent(&event) != 0) { // While events in the queue
    switch (event.type) {
    case SDL_QUIT: // Quit event
      *displayIsRunning = false;
      break;
      /*
          case SDL_MOUSEWHEEL:                          // Mousewheel event
            if (event.wheel.y > 0) {                    // Scroll up -> zoom in
              if (this->tileMap->getTileSize() == 16) { // If not already zoomed in
                this->zoomedIn  = true;
                this->zoomedOut = false;

                this->tileMap->setTileSize(32);
                this->camera->zoomIn(32, this->tileMap->getTotalXTiles(),
                                     this->tileMap->getTotalYTiles());
                break;
              }
            }
            else if (event.wheel.y < 0) {               // Scroll down -> zoom out
              if (this->tileMap->getTileSize() == 32) { // If not already zoomed out
                this->zoomedIn  = false;
                this->zoomedOut = true;

                this->tileMap->setTileSize(16);
                this->camera->zoomOut(16, this->tileMap->getTotalXTiles(),
                                      this->tileMap->getTotalYTiles());
                break;
              }
            }
      */

    default:
      break;
    }
  }

  // Still in scanning state
  return 1;
}

/**
 * Perform the appropriate action depending on which keyboard key has been pressed.
 *
 * @param None
 * @return The state the display is in after checking if any keys have been pressed
 */
int Scanning::checkKeystates() {
  const Uint8* keystates = SDL_GetKeyboardState(NULL);

  // Camera movement (arrow keys)
  if (keystates[SDL_SCANCODE_UP]) {
    return 1;
  }
  else if (keystates[SDL_SCANCODE_DOWN]) {
    return 1;
  }
  else if (keystates[SDL_SCANCODE_RIGHT]) {
    return 1;
  }
  else if (keystates[SDL_SCANCODE_LEFT]) {
    return 1;
  }
  else { // No arrow key pressed
  }

  // Pause menu
  if (keystates[SDL_SCANCODE_ESCAPE]) {
    return 2;
  }

  return 1;
}

/**
 * Update the camera and set the selected tile.
 *
 * @param None
 * @return None
 */
void Scanning::update() {
  //  writeToLogFile(this->displayGlobal.logFile, "updating in scanning");
}

/**
 * Render all scanning elements.
 *
 * @param None
 * @return None
 */
void Scanning::render() {
  SDL_RenderClear(this->displayGlobal.renderer);

  /*
  int cameraXPosition = this->camera->getXPosition();
  int cameraYPosition = this->camera->getYPosition();

  // Loop through all visible x tiles
  for (int x = 0; x < this->camera->getVisibleXTiles() + 1; x++) {
    // Loop through all visible y tiles
    for (int y = 0; y < this->camera->getVisibleYTiles() + 1; y++) {
      int currentXPosition = x + floor(cameraXPosition / 16);
      int currentYPosition = y + floor(cameraYPosition / 16);

      // Render all visible tiles
      SDL_RenderCopy(this->displayGlobal.renderer,
                     this->tileMap->getTileTexture(currentXPosition, currentYPosition),
                     NULL, &(this->camera->destinationRect[x][y]));

      // If the current tile is selected
      if (this->tileMap->getSelected(currentXPosition, currentYPosition)) {
        // Render selected texture over it
        SDL_RenderCopy(this->displayGlobal.renderer, this->selectedTexture, NULL,
                       &(camera->destinationRect[x][y]));
      }
    }
  }

  this->npcVector[0]->render();
  */

  SDL_RenderPresent(this->displayGlobal.renderer);
}

/**
 * Perform necessary actions when the scanning state is entered for the first.
 *
 * @param None
 * @return None
 */
void Scanning::enterScanning() {
  SDL_Surface* windowSurface = SDL_GetWindowSurface(this->displayGlobal.window);
  initializeTextures();

  // State has been entered once
  this->stateEntered = true;
}

/**
 * Initialize all textures in the scanning state.
 *
 * @param None
 * @return None
 */
void Scanning::initializeTextures() {
  LOG(INFO) << "Initializing textures";
  /*
  SDL_Surface* tmp_surface = IMG_Load("sprites/selected.png");
  selectedTexture = SDL_CreateTextureFromSurface(this->displayGlobal.renderer,
  tmp_surface); SDL_FreeSurface(tmp_surface);
  */
  LOG(INFO) << "Textures initialized";
}

/**
 * Check if the scanning state has been entered before.
 *
 * @param None
 * @return None
 */
bool Scanning::getStateEntered() { return this->stateEntered; }
