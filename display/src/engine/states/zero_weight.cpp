#include "zero_weight.h"
#include "../../log_files.h"

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
      this->displayGlobal, SDL_Rect{0, 50, 0, 0}, "Retry", SDL_Point{0, 0},
      [this]() { retry(); }, LogFiles::zeroWeight);
  retryButton->setCenteredHorizontal();

  std::shared_ptr<Button> overrideButton = std::make_shared<Button>(
      this->displayGlobal, SDL_Rect{0, 150, 0, 0}, "Override", SDL_Point{0, 0},
      [this]() { override(); }, LogFiles::zeroWeight);
  overrideButton->setCenteredHorizontal();

  std::shared_ptr<Button> cancelButton = std::make_shared<Button>(
      this->displayGlobal, SDL_Rect{0, 250, 0, 0}, "Cancel", SDL_Point{0, 0},
      [this]() { cancel(); }, LogFiles::zeroWeight);
  cancelButton->setCenteredHorizontal();

  this->rootElement->addElement(retryButton);
  this->rootElement->addElement(overrideButton);
  this->rootElement->addElement(cancelButton);
}

void ZeroWeight::setRetryScan(bool retryScan) { this->retryScan = retryScan; }
bool ZeroWeight::getRetryScan() { return this->retryScan; }

void ZeroWeight::retry() { this->retryScan = true; }

void ZeroWeight::override() { this->currentState = EngineState::SCANNING; }

void ZeroWeight::cancel() { this->currentState = EngineState::ITEM_LIST; }

void ZeroWeight::render() const {
  SDL_SetRenderDrawColor(this->displayGlobal.renderer, 0, 0, 0, 255); // Black background
  SDL_RenderClear(this->displayGlobal.renderer);
  this->rootElement->render();
  SDL_RenderPresent(this->displayGlobal.renderer);
}
