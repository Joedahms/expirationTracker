#include <SDL2/SDL.h>
#include <assert.h>
#include <glog/logging.h>
#include <iostream>
#include <string>

#include "../display_global.h"
#include "../text.h"
#include "scanning.h"

/**
 * @param displayGlobal Global variables
 */
Scanning::Scanning(struct DisplayGlobal displayGlobal) {
  this->displayGlobal        = displayGlobal;
  SDL_Surface* windowSurface = SDL_GetWindowSurface(this->displayGlobal.window);

  const char* fontPath               = "../display/fonts/16020_FUTURAM.ttf";
  const char* progressMessageContent = "Scanning In Progress";
  SDL_Color progressMessageColor     = {0, 255, 0, 255}; // Green
  SDL_Rect progressMessageRectangle  = {100, 100, 0, 0}; // x y w h
  std::unique_ptr<Text> progressMessage =
      std::make_unique<Text>(this->displayGlobal, fontPath, progressMessageContent, 24,
                             progressMessageColor, progressMessageRectangle);
  progressMessage->centerHorizontal(windowSurface);
  this->texts.push_back(std::move(progressMessage));
}

/**
 * Handle all events in the SDL event queue.
 *
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
      // Touch event here
    default:
      break;
    }
  }

  return SCANNING;
}

/**
 * Perform the appropriate action depending on which keyboard key has been pressed.
 *
 * @param None
 * @return The state the display is in after checking if any keys have been pressed
 */
int Scanning::checkKeystates() {
  const Uint8* keystates = SDL_GetKeyboardState(NULL);

  if (keystates[SDL_SCANCODE_ESCAPE]) {
    return PAUSE_MENU;
  }

  return SCANNING;
}

void Scanning::update() {}

/**
 * Render all scanning elements.
 *
 * @param None
 * @return None
 */
void Scanning::render() const {
  SDL_SetRenderDrawColor(this->displayGlobal.renderer, 0, 0, 0, 255); // Black background
  SDL_RenderClear(this->displayGlobal.renderer);
  renderElements();
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
