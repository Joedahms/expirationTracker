#include "loading_bar.h"

LoadingBar::LoadingBar(struct DisplayGlobal displayGlobal,
                       const SDL_Rect boundaryRectangle,
                       const int& borderThickness,
                       const float& totalTimeSeconds,
                       const float& updatePeriodMs,
                       const std::string& logFile)
    : totalTimeSeconds(totalTimeSeconds), updatePeriodMs(updatePeriodMs) {
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

  float pixelsPerMs =
      static_cast<float>(this->boundaryRectangle.w) / (this->totalTimeSeconds * 1000);
  this->pixelsPerUpdate = pixelsPerMs * this->updatePeriodMs;

  // Get time since last update
  this->currentUpdate = std::chrono::steady_clock::now();
  std::chrono::milliseconds updateDifference;
  updateDifference = std::chrono::duration_cast<std::chrono::milliseconds>(
      this->currentUpdate - this->previousUpdate);

  if (this->barRectangle.w <= this->boundaryRectangle.w) {
    if (updateDifference.count() > this->updatePeriodMs) {
      this->barRectangle.w += this->pixelsPerUpdate;
      this->previousUpdate = this->currentUpdate;
    }
  }
  else {
    this->barRectangle.w = 0;
  }

  this->barRectangle.x = this->boundaryRectangle.x + 1;
  this->barRectangle.y = this->boundaryRectangle.y + 1;
}

void LoadingBar::handleEvent(const SDL_Event& event) {
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_ESCAPE) {
      SDL_Quit();
    }
  }
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    if (checkMouseHovered()) {
      this->held = true;

      this->centerWithinParent           = false;
      this->centerVerticalWithinParent   = false;
      this->centerHorizontalWithinParent = false;
    }
  }
  else if (event.type == SDL_MOUSEMOTION) {
    if (this->held) {
      this->fixed = false;

      this->velocity.x += event.motion.x - this->previousMotion.x;
      this->velocity.y += event.motion.y - this->previousMotion.y;
      this->previousMotion.x = event.motion.x;
      this->previousMotion.y = event.motion.y;
    }
  }
  else if (event.type == SDL_MOUSEBUTTONUP) {
    this->held = false;
  }
  this->previousMotion.x = event.motion.x;
  this->previousMotion.y = event.motion.y;
}

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
