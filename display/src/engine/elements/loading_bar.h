#ifndef LOADING_BAR_H
#define LOADING_BAR_H

#include <SDL2/SDL.h>

#include "element.h"

class LoadingBar : public Element {
public:
  LoadingBar(struct DisplayGlobal displayGlobal,
             const SDL_Rect boundaryRectangle,
             const int& borderThickness,
             const float& totalTimeSeconds,
             const float& updatePeriodMs,
             const std::string& logFile);

  void update() override;
  void handleEvent(const SDL_Event& event) override;
  void render() const override;

private:
  SDL_Rect barRectangle;
  SDL_Color barColor;
  float totalTimeSeconds;
  float updatePeriodMs;
  int pixelsPerUpdate;

  bool held = false;

  SDL_Point previousMotion = {0, 0};
  float xVelocity          = 0;
  float yVelocity          = 0;
  float xAcceleration      = 0;
  float yAcceleration      = 0;
};

#endif
