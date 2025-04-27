#include <iostream>

#include "../elements/button.h"
#include "../log_files.h"
#include "zero_weight.h"

ZeroWeight::ZeroWeight(const struct DisplayGlobal& displayGlobal,
                       const EngineState& state)
    : State(displayGlobal, LogFiles::ZERO_WEIGHT, state) {
  this->logger->log("Constructing zero weight state");

  const std::string titleContent = "NO WEIGHT DETECTED ON PLATFORM";
  const SDL_Color titleColor     = {0, 255, 0, 255}; // Green
  const SDL_Rect titleRect       = {0, 50, 0, 0};
  std::unique_ptr<Text> title    = std::make_unique<Text>(
      this->displayGlobal, this->logFile, titleRect, DisplayGlobal::futuramFontPath,
      titleContent, 24, titleColor);
  title->setCenteredHorizontal();
  this->rootElement->addElement(std::move(title));

  const SDL_Point buttonTextPadding = {10, 10};

  std::shared_ptr<Button> retryButton = std::make_shared<Button>(
      this->displayGlobal, this->logFile, SDL_Rect{0, 125, 0, 0},
      "Retry Weight Measurement", buttonTextPadding, [this]() { retry(); });
  retryButton->setCenteredHorizontal();

  std::shared_ptr<Button> overrideButton = std::make_shared<Button>(
      this->displayGlobal, this->logFile, SDL_Rect{0, 200, 0, 0},
      "Override and Start Scan", buttonTextPadding, [this]() { override(); });
  overrideButton->setCenteredHorizontal();

  std::shared_ptr<Button> cancelButton =
      std::make_shared<Button>(this->displayGlobal, this->logFile, SDL_Rect{0, 275, 0, 0},
                               "Cancel", buttonTextPadding, [this]() { cancel(); });
  cancelButton->setCenteredHorizontal();

  this->rootElement->addElement(retryButton);
  this->rootElement->addElement(overrideButton);
  this->rootElement->addElement(cancelButton);
}

void ZeroWeight::setRetryScan(bool retryScan) { this->retryScan = retryScan; }
bool ZeroWeight::getRetryScan() { return this->retryScan; }

void ZeroWeight::retry() {
  this->displayHandler.zeroWeightChoiceToHardware(Messages::RETRY);
  this->currentState = this->displayHandler.startToHardware();
  this->retryScan    = true;
}

void ZeroWeight::override() { this->currentState = EngineState::SCANNING; }

void ZeroWeight::cancel() { this->currentState = EngineState::ITEM_LIST; }

void ZeroWeight::render() const {
  SDL_SetRenderDrawColor(this->displayGlobal.renderer, 0, 0, 0, 255); // Black background
  SDL_RenderClear(this->displayGlobal.renderer);
  this->rootElement->render();
  SDL_RenderPresent(this->displayGlobal.renderer);
}

void ZeroWeight::enter() {
  this->currentState = this->defaultState;
  this->retryScan    = false;
}

void ZeroWeight::exit() {
  // Override
  if (this->currentState == EngineState::SCANNING) {
    if (this->retryScan == false) {
      this->displayHandler.zeroWeightChoiceToHardware(Messages::OVERRIDE);
    }
  }
  // Cancel
  else if (this->currentState == EngineState::ITEM_LIST) {
    this->displayHandler.zeroWeightChoiceToHardware(Messages::CANCEL);
  }
  /*
  // Retry
  else if (this->retryScan) {
    this->displayHandler.zeroWeightChoiceToHardware(Messages::RETRY);
    this->retryScan = false;
    this->displayHandler.startToHardware();
  }
  */
}
