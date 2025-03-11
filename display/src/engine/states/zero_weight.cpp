#include "../../log_files.h"
#include "zero_weight.h"

ZeroWeight::ZeroWeight(struct DisplayGlobal displayGlobal)
    : logger(LogFiles::zeroWeight) {
  this->logger.log("Constructing zero weight state");
  this->currentState = EngineState::ZERO_WEIGHT;

  this->displayGlobal = displayGlobal;

  SDL_Surface* windowSurface = SDL_GetWindowSurface(this->displayGlobal.window);
  SDL_Rect rootRectangle     = {0, 0, 0, 0};
  rootRectangle.w            = windowSurface->w;
  rootRectangle.h            = windowSurface->h;
  this->rootElement          = std::make_shared<Container>(rootRectangle);

  std::shared_ptr<Button> retryButton = std::make_shared<Button>(
      this->displayGlobal, SDL_Rect{0, 0, 0, 0}, "Retry", SDL_Point{0, 0},
      [this]() { retry(); }, LogFiles::zeroWeight);

  std::shared_ptr<Button> overrideButton = std::make_shared<Button>(
      this->displayGlobal, SDL_Rect{0, 0, 0, 0}, "Override", SDL_Point{0, 0},
      [this]() { override(); }, LogFiles::zeroWeight);

  std::shared_ptr<Button> cancelButton = std::make_shared<Button>(
      this->displayGlobal, SDL_Rect{0, 0, 0, 0}, "Cancel", SDL_Point{0, 0},
      [this]() { cancel(); }, LogFiles::zeroWeight);

  this->rootElement->addElement(retryButton);
  this->rootElement->addElement(overrideButton);
  this->rootElement->addElement(cancelButton);
}

void ZeroWeight::retry() {}

void ZeroWeight::override() { this->currentState = EngineState::SCANNING; }

void ZeroWeight::cancel() { this->currentState = EngineState::ITEM_LIST; }

void ZeroWeight::render() const {
  SDL_SetRenderDrawColor(this->displayGlobal.renderer, 0, 0, 0, 255); // Black background
  SDL_RenderClear(this->displayGlobal.renderer);
  this->rootElement->render();
  SDL_RenderPresent(this->displayGlobal.renderer);
}
