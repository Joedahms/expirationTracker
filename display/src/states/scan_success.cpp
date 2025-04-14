#include <iostream>

#include "../log_files.h"
#include "scan_success.h"

ScanSuccess::ScanSuccess(struct DisplayGlobal displayGlobal)
    : logger(LogFiles::SCAN_SUCCESS) {
  this->logger.log("Constructing scan success state");
  this->currentState = EngineState::SCAN_SUCCESS;

  this->displayGlobal = displayGlobal;

  SDL_Surface* windowSurface = SDL_GetWindowSurface(this->displayGlobal.window);
  SDL_Rect rootRectangle     = {0, 0, 0, 0};
  rootRectangle.w            = windowSurface->w;
  rootRectangle.h            = windowSurface->h;
  this->rootElement          = std::make_shared<Container>(rootRectangle);

  const std::string successMessageContent = "Scan Successful!";
  const SDL_Color successMessageColor     = {0, 255, 0, 255}; // Green
  const SDL_Rect successMessageRectangle  = {0, 100, 0, 0};
  std::unique_ptr<Text> successMessage    = std::make_unique<Text>(
      this->displayGlobal, successMessageRectangle, DisplayGlobal::futuramFontPath,
      successMessageContent, 24, successMessageColor);
  successMessage->setCenteredHorizontal();
  this->rootElement->addElement(std::move(successMessage));

  std::shared_ptr<Button> okButton = std::make_shared<Button>(
      this->displayGlobal, SDL_Rect{0, 50, 0, 0}, "OK", SDL_Point{0, 0},
      [this]() { retry(); }, LogFiles::SCAN_SUCCESS);
  okButton->setCenteredHorizontal();
  this->rootElement->addElement(okButton);
}

void ZeroWeight::render() const {
  SDL_SetRenderDrawColor(this->displayGlobal.renderer, 0, 0, 0, 255); // Black background
  SDL_RenderClear(this->displayGlobal.renderer);
  this->rootElement->render();
  SDL_RenderPresent(this->displayGlobal.renderer);
}
