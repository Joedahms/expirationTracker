#include <SDL2/SDL.h>
#include <assert.h>
#include <glog/logging.h>
#include <iostream>
#include <string>

#include "../display_global.h"
#include "../elements/button.h"
#include "../elements/flappy_food/flappy_food.h"
#include "../elements/loading_bar.h"
#include "../elements/text.h"
#include "../log_files.h"
#include "scanning.h"

/**
 * @param displayGlobal Global variables
 */
Scanning::Scanning(const struct DisplayGlobal& displayGlobal, const EngineState& state)
    : State(displayGlobal, LogFiles::SCANNING, state) {
  this->logger->log("Constructing scanning state");

  this->logger->log("Constructing scan message");
  const std::string progressMessageContent = "Scanning In Progress";
  const SDL_Color progressMessageColor     = {0, 255, 0, 255}; // Green
  const SDL_Rect progressMessageRectangle  = {0, 0, 0, 0};
  std::unique_ptr<Text> progressMessage    = std::make_unique<Text>(
      this->displayGlobal, this->logFile, progressMessageRectangle,
      DisplayGlobal::futuramFontPath, progressMessageContent, 24, progressMessageColor);
  progressMessage->setCenteredHorizontal();
  this->rootElement->addElement(std::move(progressMessage));
  this->logger->log("Scan message constructed");

  this->logger->log("Constructing cancel scan button");
  SDL_Rect cancelScanButtonRectangle       = {280, 50, 0, 0};
  std::unique_ptr<Button> cancelScanButton = std::make_unique<Button>(
      this->displayGlobal, this->logFile, cancelScanButtonRectangle, "Cancel Scan",
      SDL_Point{10, 10},
      [this]() { this->currentState = EngineState::CANCEL_SCAN_CONFIRMATION; });
  this->rootElement->addElement(std::move(cancelScanButton));
  this->logger->log("Cancel scan button constructed");

  this->logger->log("Constructing start game button");
  SDL_Rect startGameRect            = {0, 50, 0, 0};
  std::shared_ptr<Button> startGame = std::make_shared<Button>(
      this->displayGlobal, this->logFile, startGameRect, "Start Game", SDL_Point{10, 10},
      [this] { this->flappyFood->start(); });
  startGame->setCenteredHorizontal();
  this->rootElement->addElement(startGame);
  this->logger->log("Start game button constructed");

  this->logger->log("Constructing reset game button");
  SDL_Rect resetGameRect            = {600, 50, 0, 0};
  std::shared_ptr<Button> resetGame = std::make_shared<Button>(
      this->displayGlobal, this->logFile, resetGameRect, "Reset Game", SDL_Point{10, 10},
      [this] { this->flappyFood->reset(); });
  this->rootElement->addElement(resetGame);
  this->logger->log("Reset game button constructed");

  this->logger->log("Constructing loading bar");
  SDL_Rect loadingBarRectangle           = {0, 100, 200, 30};
  int loadingBarBorderThickness          = 3;
  float totalTimeSeconds                 = 20;
  float updatePeriodMs                   = 100;
  std::unique_ptr<LoadingBar> loadingBar = std::make_unique<LoadingBar>(
      this->displayGlobal, this->logFile, loadingBarRectangle, loadingBarBorderThickness,
      totalTimeSeconds, updatePeriodMs);
  loadingBar->setCenteredHorizontal();
  rootElement->addElement(std::move(loadingBar));
  this->logger->log("Loading bar constructed");

  this->flappyFood = std::make_shared<FlappyFood>(
      this->displayGlobal, this->logFile,
      SDL_Rect{0, 0, this->windowSurface->w, this->windowSurface->h});
  this->rootElement->addElement(flappyFood);

  this->logger->log("Constructed scanning state");
}

void Scanning::handleEvents(bool* displayIsRunning) {
  SDL_Event event;
  while (SDL_PollEvent(&event) != 0) { // While there are events in the queue
    if (event.type == SDL_QUIT) {
      *displayIsRunning = false;
      break;
    }
    else {
      this->rootElement->handleEvent(event);
    }
  }

  this->currentState = this->displayHandler.checkDetectionResults(this->currentState);
}

/**
 * Render all scanning elements.
 *
 * @param None
 * @return None
 */
void Scanning::render() const {
  SDL_SetRenderDrawColor(this->displayGlobal.renderer, 0, 0, 0, 255); // Black background
  SDL_RenderClear(this->displayGlobal.renderer);
  this->rootElement->render();
  SDL_RenderPresent(this->displayGlobal.renderer);
}

void Scanning::enter() {
  std::cout << "here" << std::endl;
  this->currentState = this->defaultState;
  // this->displayHandler.startToHardware();
}

void Scanning::exit() {}
