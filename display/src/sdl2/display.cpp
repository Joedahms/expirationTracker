#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <assert.h>
#include <glog/logging.h>
#include <iostream>

#include "../../../pipes.h"
#include "display.h"
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
Display::Display(const char* windowTitle,
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

  initializeSdl(this->displayGlobal.window);

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
SDL_Window* Display::setupWindow(const char* windowTitle,
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
void Display::initializeSdl(SDL_Window* window) {
  // Initialize SDL
  LOG(INFO) << "Initializing SDL";
  try {
    int sdlInitReturn = SDL_Init(SDL_INIT_EVERYTHING);
    if (sdlInitReturn != 0) {
      throw;
    }
  } catch (...) {
    LOG(FATAL) << "Failed to initialize SDL";
    exit(1);
  }
  LOG(INFO) << "SDL initialized";

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
void Display::checkState() {
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
    break;
  }
}

/**
 * Check the current state, and call that state's handle events method.
 *
 * @param None
 * @return None
 */
void Display::handleEvents(int* sdlToDisplay, int* displayToSdl) {
  switch (this->state) {
  case MAIN_MENU:
    this->state = this->mainMenu->handleEvents(&this->displayIsRunning);
    if (this->state == SCANNING) {
      writeString(sdlToDisplay[WRITE], START_SCAN);
    }
    break;

  case SCANNING:
    {
      this->state = this->scanning->handleEvents(&this->displayIsRunning);
      std::string fromDisplay;
      if (fromDisplay == readString(displayToSdl[READ])) {
        std::cout << "yay" << std::endl;
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
void Display::checkKeystates() {
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
void Display::update() {
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
    break;
  }
}

/**
 * Check current state and call that state's function to render.
 *
 * @param None
 * @return None
 */
void Display::renderState() {
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
    break;
  }
}

/**
 * Free SDL resources and quit.
 *
 * @param None
 * @return None
 */
void Display::clean() {
  SDL_DestroyWindow(this->displayGlobal.window);
  SDL_DestroyRenderer(this->displayGlobal.renderer);
  SDL_Quit();
  LOG(INFO) << "Display cleaned";
}
