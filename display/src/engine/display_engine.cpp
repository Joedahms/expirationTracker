#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <assert.h>
#include <glog/logging.h>
#include <iostream>

#include "../../../pipes.h"
#include "display_engine.h"
#include "display_global.h"
#include "states/state.h"

/**
 * @param windowTitle Title of the display window
 * @param windowXPosition X position of the display window
 * @param windowYPostition Y position of the display window
 * @param screenWidth Width of the display window in pixels
 * @param screenHeight Height of the display window in pixels
 * @param fullscreen Whether or not the display window should be fullscreen
 * @param displayGlobal Global display variables
 */
DisplayEngine::DisplayEngine(const char* windowTitle,
                             int windowXPosition,
                             int windowYPosition,
                             int screenWidth,
                             int screenHeight,
                             bool fullscreen,
                             struct DisplayGlobal displayGlobal) {
  this->displayGlobal = displayGlobal;
  this->displayGlobal.window =
      setupWindow(windowTitle, windowXPosition, windowYPosition, screenWidth,
                  screenHeight, fullscreen); // Setup the SDL display window

  initializeEngine(this->displayGlobal.window);

  // Initialize states
  this->mainMenu  = std::make_unique<MainMenu>(this->displayGlobal);
  this->scanning  = std::make_unique<Scanning>(this->displayGlobal);
  this->pauseMenu = std::make_unique<PauseMenu>(this->displayGlobal);
  this->itemList  = std::make_unique<ItemList>(this->displayGlobal);

  displayIsRunning = true;
}

/**
 * Setup the SDL display window.
 *
 * @param windowTitle The name of the window. Is what is displayed on the top bezel of
 * the window when the display is running.
 * @param windowXPosition The X position of the window on the user's screen.
 * @param windowYPosition The Y position of the window on the user's screen.
 * @param screenWidth The width of the screen in pixels.
 * @param screenHeight The height of the screen in pixels.
 * @param fullscreen Whether or not the display window should be fullscreen.
 * @return Pointer to the SDL display window.
 */
SDL_Window* DisplayEngine::setupWindow(const char* windowTitle,
                                       int windowXPosition,
                                       int windowYPosition,
                                       int screenWidth,
                                       int screenHeight,
                                       bool fullscreen) {
  LOG(INFO) << "Creating SDL display window";

  int flags = 0;
  if (fullscreen) {
    flags = SDL_WINDOW_FULLSCREEN;
  }

  try {
    return SDL_CreateWindow(windowTitle, windowXPosition, windowYPosition, screenWidth,
                            screenHeight, flags);
  } catch (...) {
    LOG(FATAL) << "Error setting up SDL display window";
  }

  LOG(INFO) << "SDL display window created";
}

/**
 * Setup SDL, the renderer, and TTF. Renderer is part of the global display objects.
 *
 * @param window The SDL display window
 * @return None
 */
void DisplayEngine::initializeEngine(SDL_Window* window) {
  LOG(INFO) << "Initializing engine";
  try {
    int sdlInitReturn = SDL_Init(SDL_INIT_EVERYTHING);
    if (sdlInitReturn != 0) {
      throw;
    }
  } catch (...) {
    LOG(FATAL) << "Failed to initialize engine";
    exit(1);
  }
  LOG(INFO) << "engine initialized";

  // Create renderer
  LOG(INFO) << "Creating renderer";
  try {
    this->displayGlobal.renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!this->displayGlobal.renderer) {
      throw;
    }
    SDL_SetRenderDrawColor(this->displayGlobal.renderer, 255, 255, 255, 255);
  } catch (...) {
    LOG(FATAL) << "Error creating renderer";
    exit(1);
  }
  LOG(INFO) << "Renderer created";

  // Initialize TTF
  LOG(INFO) << "Initializing TTF";
  try {
    int ttfInitReturn = TTF_Init();
    if (ttfInitReturn == -1) {
      throw;
    }
  } catch (...) {
    LOG(FATAL) << "Failed to initialize TTF";
    exit(1);
  }
  LOG(INFO) << "TTF initialized";
}

/**
 * Check which state the display is in. If there was a state switch and the current
 * state has not been entered before, run its enter method.
 *
 * @param None
 * @return None
 */
void DisplayEngine::checkState() {
  switch (this->state) {
  case MAIN_MENU:
    break;

  case SCANNING:
    if (!this->scanning->getStateEntered()) {
      this->scanning->enterScanning();
    }
    break;

  case PAUSE_MENU:
    break;

  case ITEM_LIST:
    break;

  default:
    LOG(FATAL) << "Invalid state";
    break;
  }
}

/**
 * Check the current state, and call that state's handle events method.
 *
 * @param engineToDisplay Pipe from the display engine to the display
 * @param displayToEngine Pipe from the display to the displayEngine
 * @return None
 */
void DisplayEngine::handleEvents(int* engineToDisplay, int* displayToEngine) {
  switch (this->state) {
  case MAIN_MENU:
    this->state = this->mainMenu->handleEvents(&this->displayIsRunning);
    if (this->state == SCANNING) {
      LOG(INFO) << "Scan initialized, engine switching to scanning state";
      writeString(engineToDisplay[WRITE], START_SCAN);
    }
    break;

  case SCANNING:
    {
      this->state = this->scanning->handleEvents(&this->displayIsRunning);

      struct timeval timeout;
      timeout.tv_sec  = 0;
      timeout.tv_usec = 100;

      int pipeToRead = displayToEngine[READ];
      fd_set readPipeSet;
      FD_ZERO(&readPipeSet);
      FD_SET(pipeToRead, &readPipeSet);

      // Check pipe for data
      int pipeReady = select(pipeToRead + 1, &readPipeSet, NULL, NULL, &timeout);

      if (pipeReady == -1) {
        LOG(FATAL) << "Select error";
      }
      else if (pipeReady == 0) { // No data available
        break;
      }
      if (FD_ISSET(pipeToRead, &readPipeSet)) { // Data available
        std::string fromDisplay = readString(displayToEngine[READ]);
        if (fromDisplay == "ID successful") {
          LOG(INFO) << "New item received, switching to item list state";
          this->state = ITEM_LIST;
        }
      }
      break;
    }

  case PAUSE_MENU:
    this->state = this->pauseMenu->handleEvents(&this->displayIsRunning);
    break;

  case ITEM_LIST:
    this->state = this->itemList->handleEvents(&this->displayIsRunning);
    break;

  default:
    LOG(FATAL) << "Invalid state";
    break;
  }
}

/**
 * Check the current state of the display and call that state's method to check the
 * key states
 *
 * @param None
 * @return None
 */
void DisplayEngine::checkKeystates() {
  switch (this->state) {
  case MAIN_MENU:
    break;

  case SCANNING:
    this->state = this->scanning->checkKeystates();
    break;

  case PAUSE_MENU:
    break;

  case ITEM_LIST:
    this->state = this->itemList->checkKeystates();
    break;

  default:
    LOG(FATAL) << "Invalid state";
    break;
  }
}

/**
 * Not implemented but may want to add:
 * First check if it's time to update. If it is, reset the time since last
 * update.
 *
 * @param None
 * @return None
 */
void DisplayEngine::update() {
  switch (this->state) {
  case MAIN_MENU:
    this->mainMenu->update();
    break;

  case SCANNING:
    this->scanning->update();
    break;

  case PAUSE_MENU:
    this->pauseMenu->update();
    break;

  case ITEM_LIST:
    this->itemList->update();
    break;

  default:
    LOG(FATAL) << "Invalid state";
    break;
  }
}

/**
 * Check current state and call that state's function to render.
 *
 * @param None
 * @return None
 */
void DisplayEngine::renderState() {
  switch (this->state) {
  case MAIN_MENU:
    this->mainMenu->render();
    break;

  case SCANNING:
    this->scanning->render();
    break;

  case PAUSE_MENU:
    this->pauseMenu->render();
    break;

  case ITEM_LIST:
    this->itemList->render();
    break;

  default:
    LOG(FATAL) << "Invalid state";
    break;
  }
}

/**
 * Free SDL resources and quit.
 *
 * @param None
 * @return None
 */
void DisplayEngine::clean() {
  SDL_DestroyWindow(this->displayGlobal.window);
  SDL_DestroyRenderer(this->displayGlobal.renderer);
  SDL_Quit();
  LOG(INFO) << "DisplayEngine cleaned";
}
