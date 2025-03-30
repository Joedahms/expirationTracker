#include <SDL2/SDL.h>
#include <assert.h>
#include <glog/logging.h>
#include <iostream>
#include <random>
#include <string>

#include "../../log_files.h"
#include "../display_global.h"
#include "../elements/bird.h"
#include "../elements/loading_bar.h"
#include "../elements/obstacle.h"
#include "../elements/text.h"
#include "scanning.h"

/**
 * @param displayGlobal Global variables
 */
Scanning::Scanning(struct DisplayGlobal displayGlobal) : logger(LogFiles::SCANNING) {
  this->logger.log("Constructing scanning state");

  this->currentState         = EngineState::SCANNING;
  this->displayGlobal        = displayGlobal;
  SDL_Surface* windowSurface = SDL_GetWindowSurface(this->displayGlobal.window);
  this->windowWidth          = windowSurface->w;
  this->windowHeight         = windowSurface->h;

  SDL_Rect rootRectangle = {0, 0, 0, 0};
  rootRectangle.w        = this->windowWidth;
  rootRectangle.h        = this->windowHeight;
  this->rootElement      = std::make_unique<Container>(rootRectangle);

  const char* progressMessageContent    = "Scanning In Progress";
  SDL_Color progressMessageColor        = {0, 255, 0, 255}; // Green
  SDL_Rect progressMessageRectangle     = {0, 100, 0, 0};
  std::unique_ptr<Text> progressMessage = std::make_unique<Text>(
      this->displayGlobal, progressMessageRectangle, DisplayGlobal::futuramFontPath,
      progressMessageContent, 24, progressMessageColor);

  progressMessage->setCenteredHorizontal();
  this->rootElement->addElement(std::move(progressMessage));

  SDL_Rect cancelScanButtonRectangle       = {0, 150, 0, 0};
  std::unique_ptr<Button> cancelScanButton = std::make_unique<Button>(
      this->displayGlobal, cancelScanButtonRectangle, "Cancel Scan", SDL_Point{10, 10},
      [this]() { this->currentState = EngineState::CANCEL_SCAN_CONFIRMATION; },
      LogFiles::SCANNING);
  cancelScanButton->setCenteredHorizontal();
  rootElement->addElement(std::move(cancelScanButton));

  SDL_Rect loadingBarRectangle           = {0, 200, 200, 30};
  int loadingBarBorderThickness          = 3;
  float totalTimeSeconds                 = 20;
  float updatePeriodMs                   = 100;
  std::unique_ptr<LoadingBar> loadingBar = std::make_unique<LoadingBar>(
      this->displayGlobal, loadingBarRectangle, loadingBarBorderThickness,
      totalTimeSeconds, updatePeriodMs, LogFiles::SCANNING);
  loadingBar->setCenteredHorizontal();
  rootElement->addElement(std::move(loadingBar));

  SDL_Rect birdRectangle     = {0, 0, 32, 32};
  std::unique_ptr<Bird> bird = std::make_unique<Bird>(this->displayGlobal, birdRectangle);
  rootElement->addElement(std::move(bird));

  initializeObstacles();

  this->logger.log("Constructed scanning state");
}

/**
 * Perform the appropriate action depending on which keyboard key has been pressed.
 *
 * @param None
 * @return The state the display is in after checking if any keys have been pressed
 */
EngineState Scanning::checkKeystates() {
  const Uint8* keystates = SDL_GetKeyboardState(NULL);
  return EngineState::SCANNING;
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

void Scanning::initializeObstacles() {
  int totalObstaclePairs =
      this->windowWidth / (this->obstacleWidth + this->horizontalObstacleGap);

  totalObstaclePairs++;
  int respawnOffset =
      this->windowWidth % (this->obstacleWidth + this->horizontalObstacleGap);

  int xPosition = this->windowWidth;
  for (int i = 0; i < totalObstaclePairs; i++) {
    initializeObstaclePair(xPosition, respawnOffset);
    xPosition += this->obstacleWidth + this->horizontalObstacleGap;
  }
}

void Scanning::initializeObstaclePair(int xPosition, int respawnOffset) {
  std::random_device r;

  std::default_random_engine e1(r());
  std::uniform_int_distribution<int> uniform_dist(this->obstacleMinHeight,
                                                  this->obstacleMaxHeight);
  int obstacleHeight = uniform_dist(e1);

  int yPosition = this->windowHeight - obstacleHeight;

  SDL_Rect bottomObstacleRect = {xPosition, yPosition, this->obstacleWidth,
                                 obstacleHeight};
  std::unique_ptr<Obstacle> bottomObstacle =
      std::make_unique<Obstacle>(this->displayGlobal, bottomObstacleRect, respawnOffset);
  rootElement->addElement(std::move(bottomObstacle));

  obstacleHeight = obstaclePairHeight - obstacleHeight - verticalObstacleGap;
  yPosition      = yPosition - this->verticalObstacleGap - obstacleHeight;

  SDL_Rect topObstacleRect = {xPosition, yPosition, this->obstacleWidth, obstacleHeight};
  std::unique_ptr<Obstacle> topObstacle =
      std::make_unique<Obstacle>(this->displayGlobal, topObstacleRect, respawnOffset);
  rootElement->addElement(std::move(topObstacle));
}
