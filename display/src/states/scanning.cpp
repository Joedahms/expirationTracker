#include <SDL2/SDL.h>
#include <assert.h>
#include <glog/logging.h>
#include <iostream>
#include <string>

#include "../display_global.h"
#include "../elements/button.h"
#include "../elements/flappy_food/bird.h"
#include "../elements/flappy_food/obstacle.h"
#include "../elements/flappy_food/obstacle_pair.h"
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
  SDL_Rect cancelScanButtonRectangle       = {0, 50, 0, 0};
  std::unique_ptr<Button> cancelScanButton = std::make_unique<Button>(
      this->displayGlobal, this->logFile, cancelScanButtonRectangle, "Cancel Scan",
      SDL_Point{10, 10},
      [this]() { this->currentState = EngineState::CANCEL_SCAN_CONFIRMATION; });
  cancelScanButton->setCenteredHorizontal();
  rootElement->addElement(std::move(cancelScanButton));
  this->logger->log("Cancel scan button constructed");

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

  this->logger->log("Constructing bird");
  SDL_Rect birdRect = {20, 0, 32, 32};
  birdRect.y        = this->windowSurface->h - birdRect.h;
  std::unique_ptr<Bird> bird =
      std::make_unique<Bird>(this->displayGlobal, this->logFile, birdRect);
  birdPtr = bird.get();
  rootElement->addElement(std::move(bird));
  this->logger->log("Bird constructed");

  initializeObstacles();

  this->logger->log("Constructing score text");
  const std::string scoreTextContent = "0";
  const SDL_Color scoreTextColor     = {0, 255, 0, 255}; // Green
  const SDL_Rect scoreTextRect       = {0, 150, 0, 0};
  this->scoreText = std::make_shared<Text>(this->displayGlobal, this->logFile,
                                           scoreTextRect, DisplayGlobal::futuramFontPath,
                                           scoreTextContent, 24, scoreTextColor);
  this->scoreText->setCenteredHorizontal();
  this->rootElement->addElement(scoreText);
  this->logger->log("Score text constructed");

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

void Scanning::update() {
  this->rootElement->update();

  std::vector<SDL_Rect> boundaryRectangles = getBoundaryRectangles();
  this->rootElement->checkCollision(boundaryRectangles);
  handleBirdCollision();

  SDL_Point birdRelative = this->birdPtr->getPositionRelativeToParent();
  if (birdRelative.y < this->windowSurface->h - 400) {
    this->birdPtr->setVelocity(Velocity{0, 0});
    birdRelative.y++;
    this->birdPtr->setPositionRelativeToParent(birdRelative);
  }
  for (const auto& obstaclePair : this->obstaclePairs) {
    if (obstaclePair->scored) {
      continue;
    }

    SDL_Rect pairRect = obstaclePair->getBoundaryRectangle();
    if (birdRelative.x >= pairRect.x + pairRect.w) {
      this->score++;
      this->scoreText->setContent(std::to_string(this->score));
      obstaclePair->scored = true;
    }
  }
}

void Scanning::initializeObstacles() {
  const int pairWidth         = 40;
  const int pairHeight        = 400;
  const int horizontalPairGap = 150;
  const int minHeight         = 20;
  const int verticalGap       = 150;

  const int windowWidth  = this->windowSurface->w;
  const int windowHeight = this->windowSurface->h;
  int xPosition          = windowWidth;
  const int yPosition    = windowHeight - pairHeight;

  const int totalPairs    = (windowWidth / (pairWidth + horizontalPairGap)) + 1;
  const int respawnOffset = windowWidth % (pairWidth + horizontalPairGap);

  for (int i = 0; i < totalPairs; i++) {
    SDL_Rect boundaryRectangle = {xPosition, yPosition, pairWidth, pairHeight};

    std::shared_ptr<ObstaclePair> obstaclePair = std::make_shared<ObstaclePair>(
        this->displayGlobal, this->logFile, boundaryRectangle, windowWidth, respawnOffset,
        minHeight, verticalGap);

    obstaclePairs.push_back(obstaclePair);

    this->rootElement->addElement(std::move(obstaclePair));
    xPosition += pairWidth + horizontalPairGap;
  }
}

std::vector<SDL_Rect> Scanning::getBoundaryRectangles() {
  std::vector<SDL_Rect> boundaryRectangles;

  this->rootElement->addBoundaryRectangle(boundaryRectangles);

  return boundaryRectangles;
}

void Scanning::handleBirdCollision() {
  if (this->birdPtr->getHasCollided()) {
    this->birdPtr->setVelocity(Velocity{0, 0});
    for (const auto& obstaclePair : this->obstaclePairs) {
      obstaclePair->setVelocity(Velocity{0, 0});
    }
  }
}

void Scanning::exit() {}
