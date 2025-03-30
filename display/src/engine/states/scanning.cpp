#include <SDL2/SDL.h>
#include <assert.h>
#include <glog/logging.h>
#include <iostream>
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

  SDL_Rect rootRectangle = {0, 0, 0, 0};
  rootRectangle.w        = windowSurface->w;
  rootRectangle.h        = windowSurface->h;
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

  int windowHeight;
  int windowWidth;
  SDL_GetWindowSize(this->displayGlobal.window, &windowWidth, &windowHeight);
  int height = windowHeight - 70;
  std::cout << "height: " << height << std::endl;

  SDL_Rect obstacleRect = {windowWidth - 40, height, 40, 70};
  std::unique_ptr<Obstacle> obstacle =
      std::make_unique<Obstacle>(this->displayGlobal, obstacleRect);
  rootElement->addElement(std::move(obstacle));

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
