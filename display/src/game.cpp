#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <assert.h>
#include <glog/logging.h>
#include <iostream>

#include "game.h"
#include "game_global.h"

/**
 * Input:
 * - windowTitle - Title of the game window
 * - windowXPosition - X position of the game window
 * - windowYPostition - Y position of the game window
 * - screenWidth - Width of the game window
 * - screenHeight - Height of the game window
 * - fullscreen - Whether or not the game window should be fullscreen
 * - gameGlobal - Global game variables
 * Output: None
 */
Game::Game(const char* windowTitle,
           int windowXPosition,
           int windowYPosition,
           int screenWidth,
           int screenHeight,
           bool fullscreen,
           struct GameGlobal gameGlobal) {
  this->gameGlobal = gameGlobal;
  this->gameGlobal.window =
      setupWindow(windowTitle, windowXPosition, windowYPosition, screenWidth,
                  screenHeight, fullscreen); // Setup the SDL game window

  initializeSdl(this->gameGlobal.window);

  // Initialize states
  this->mainMenu  = std::make_unique<MainMenu>(this->gameGlobal);
  this->gameplay  = std::make_unique<Gameplay>(this->gameGlobal);
  this->pauseMenu = std::make_unique<PauseMenu>(this->gameGlobal);

  gameIsRunning = true;
}

/**
 * Setup the SDL game window.
 *
 * Input:
 * - windowTitle - The name of the window. Is what is displayed on the top bezel of
 * the window when the game is running.
 * - windowXPosition - The X position of the window on the user's screen.
 * - windowYPosition - The Y position of the window on the user's screen.
 * - screenWidth - The width of the screen in pixels.
 * - screenHeight - The height of the screen in pixels.
 * @param fullscreen - Whether or not the game window should be fullscreen.
 * @return - Pointer to the SDL game window.
 */
SDL_Window* Game::setupWindow(const char* windowTitle,
                              int windowXPosition,
                              int windowYPosition,
                              int screenWidth,
                              int screenHeight,
                              bool fullscreen) {
  LOG(INFO) << "Creating SDL game window";

  int flags = 0;
  if (fullscreen) {
    flags = SDL_WINDOW_FULLSCREEN;
  }

  try {
    return SDL_CreateWindow(windowTitle, windowXPosition, windowYPosition, screenWidth,
                            screenHeight, flags);
  } catch (...) {
    LOG(FATAL) << "Error setting up SDL game window";
    exit(1);
  }

  LOG(INFO) << "SDL game window created";
}

/**
 * Setup SDL, the renderer, and TTF. Renderer is part of the global game objects.
 *
 * @param window - The SDL game window.
 * @return - None
 */
void Game::initializeSdl(SDL_Window* window) {
  // Initialize SDL
  //  writeToLogFile(this->gameGlobal.logFile, "Initializing SDL...");
  try {
    int sdlInitReturn = SDL_Init(SDL_INIT_EVERYTHING);
    if (sdlInitReturn != 0) {
      throw;
    }
  } catch (...) {
    //   writeToLogFile(this->gameGlobal.logFile, "Failed to initialize SDL");
    exit(1);
  }
  // writeToLogFile(this->gameGlobal.logFile, "SDL initialized");

  // Create renderer
  // writeToLogFile(this->gameGlobal.logFile, "Creating renderer");
  try {
    this->gameGlobal.renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!this->gameGlobal.renderer) {
      throw;
    }
    SDL_SetRenderDrawColor(this->gameGlobal.renderer, 255, 255, 255, 255);
  } catch (...) {
    // writeToLogFile(this->gameGlobal.logFile, "Error creating renderer");
    exit(1);
  }
  // writeToLogFile(this->gameGlobal.logFile, "Renderer created");

  // Initialize TTF
  // writeToLogFile(this->gameGlobal.logFile, "Initializing TTF...");
  try {
    int ttfInitReturn = TTF_Init();
    if (ttfInitReturn == -1) {
      throw;
    }
  } catch (...) {
    // writeToLogFile(this->gameGlobal.logFile, "Failed to initialize TTF");
    exit(1);
  }
  // writeToLogFile(this->gameGlobal.logFile, "TTF initialized");
}

/**
 * Check which state the game is in. If there was a state switch and the current
 * state has not been entered before, run its enter method.
 *
 * @param - None
 * @return - None
 */
void Game::checkState() {
  switch (this->state) {
  case 0: // Main menu
    break;

  case 1: // Gameplay
    if (!this->gameplay->getStateEntered()) {
      this->gameplay->enterGameplay();
    }
    break;

  case 2: // Pause menu
    break;

  default:
    break;
  }
}

/**
 * Check the current state, and call that state's handle events method.
 *
 * @param - None
 * @return - None
 */
void Game::handleEvents() {
  switch (this->state) {
  case 0: // Main menu
    this->state = this->mainMenu->handleEvents(&this->gameIsRunning);
    break;

  case 1: // Gameplay
    this->state = this->gameplay->handleEvents(&this->gameIsRunning);
    break;

  case 2: // Pause menu
    this->state = this->pauseMenu->handleEvents(&this->gameIsRunning);
    break;

  default:
    break;
  }
}

/**
 * Check the current state of the game and call that state's method to check the
 * key states
 *
 * @param - None
 * @return - None
 */
void Game::checkKeystates() {
  switch (this->state) {
  case 0: // Main menu
    break;

  case 1: // Gameplay
    this->state = this->gameplay->checkKeystates();
    break;

  case 2: // Pause menu
    break;

  default:
    break;
  }
}

/**
 * First check if it's time to update. If it is, reset the time since last
 * update. Then check the current state and call that state's function to update.
 *
 * @param - None
 * @return - None
 */
void Game::update() {
  switch (this->state) { // Check current state
  case 0:                // Main menu
    break;

  case 1: // Gameplay
    this->gameplay->update();
    break;

  case 2: // Pause menu
    break;

  default:
    break;
  }
}

/**
 * Check current state and call that state's function to render.
 *
 * @param - None
 * @return - None
 */
void Game::renderState() {
  switch (this->state) {
  case 0: // Main menu
    this->mainMenu->render();
    break;

  case 1: // Gameplay
    this->gameplay->render();
    break;

  case 2: // Pause menu
    this->pauseMenu->render();
    break;

  default:
    break;
  }
}

/**
 * Free SDL resources and quit.
 *
 * @param - None
 * @return - None
 */
void Game::clean() {
  SDL_DestroyWindow(this->gameGlobal.window);
  SDL_DestroyRenderer(this->gameGlobal.renderer);
  SDL_Quit();
  // writeToLogFile(this->gameGlobal.logFile, "Game cleaned");
}
