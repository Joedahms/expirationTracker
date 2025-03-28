#include "loading_bar.h"

LoadingBar::LoadingBar(struct DisplayGlobal displayGlobal,
                       const SDL_Rect boundaryRectangle,
                       const int& borderThickness,
                       const std::string& logFile) {
  this->logger          = std::make_unique<Logger>(logFile);
  this->displayGlobal   = displayGlobal;
  this->borderThickness = borderThickness;
  setupPosition(boundaryRectangle);

  this->barRectangle = {this->boundaryRectangle.x + 1, this->boundaryRectangle.y, 0,
                        this->boundaryRectangle.h - 1};
  this->barColor     = {0, 255, 0, 255};

  this->previousUpdate = std::chrono::steady_clock::now();
  addBorder(this->borderThickness);
}

void LoadingBar::update() {
  if (parent) {
    hasParentUpdate();
  }

  this->barRectangle.x = this->boundaryRectangle.x + 1;
  this->barRectangle.y = this->boundaryRectangle.y + 1;

  // Get time since last update
  this->currentUpdate = std::chrono::steady_clock::now();
  std::chrono::seconds updateDifference;
  updateDifference = std::chrono::duration_cast<std::chrono::seconds>(
      this->currentUpdate - this->previousUpdate);

  // 1 or more seconds since last update
  if (updateDifference.count() > 0.5) {
    this->barRectangle.w++;
    this->previousUpdate = this->currentUpdate;
  }
}

void LoadingBar::handleEvent(const SDL_Event& event) {}

void LoadingBar::render() const {
  // Border
  if (this->hasBorder) {
    renderBorder();
  }

  // Bar
  SDL_SetRenderDrawColor(this->displayGlobal.renderer, barColor.r, barColor.g, barColor.b,
                         barColor.a);
  SDL_RenderFillRect(this->displayGlobal.renderer, &this->barRectangle);
}
