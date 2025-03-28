#ifndef LOADING_BAR_H
#define LOADING_BAR_H

#include <SDL2/SDL.h>

#include "element.h"

class LoadingBar : public Element {
public:
  LoadingBar(struct DisplayGlobal displayGlobal,
             const SDL_Rect boundaryRectangle,
             const int& borderThickness,
             const std::string& logFile);

  void update() override;
  void handleEvent(const SDL_Event& event) override;
  void render() const override;

private:
  SDL_Rect barRectangle;
  SDL_Color barColor;
};

#endif
