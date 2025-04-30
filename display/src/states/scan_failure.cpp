#include "../elements/button.h"
#include "../elements/text.h"
#include "scan_failure.h"

ScanFailure::ScanFailure(const struct DisplayGlobal& displayGlobal,
                         const EngineState& state)
    : State(displayGlobal, LogFiles::SCAN_FAILURE, state) {
  this->logger->log("Constructing scan failure state");

  const std::string messageContent = "Scan failed";
  const SDL_Color messageColor     = {0, 255, 0, 255}; // Green
  const SDL_Rect messageRectangle  = {0, 100, 0, 0};
  std::unique_ptr<Text> message    = std::make_unique<Text>(
      this->displayGlobal, this->logFile, messageRectangle,
      DisplayGlobal::futuramFontPath, messageContent, 24, messageColor);
  message->setCenteredHorizontal();
  this->rootElement->addElement(std::move(message));

  std::shared_ptr<Button> okButton = std::make_shared<Button>(
      this->displayGlobal, this->logFile, SDL_Rect{0, 200, 0, 0}, "OK", SDL_Point{10, 10},
      [this]() { this->currentState = EngineState::ITEM_LIST; });
  okButton->setCenteredHorizontal();
  this->rootElement->addElement(okButton);
}

/**
 * Render
 *
 * @param None
 * @return None
 */
void ScanFailure::render() const {
  SDL_SetRenderDrawColor(this->displayGlobal.renderer, 0, 0, 0, 255); // Black background
  SDL_RenderClear(this->displayGlobal.renderer);
  this->rootElement->render();
  SDL_RenderPresent(this->displayGlobal.renderer);
}

void ScanFailure::exit() {}
