#include <SDL2/SDL.h>
#include <memory>
#include <vector>

#include "panel.h"
#include "scroll_box.h"

ScrollBox::ScrollBox(SDL_Rect rect, std::vector<std::unique_ptr<Panel>> p) {
  this->rectangle = rect;
  this->panels    = std::move(p);
}

void ScrollBox::render() {}
