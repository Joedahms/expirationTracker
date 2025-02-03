#include <SDL2/SDL.h>
#include <memory>
#include <vector>

#include "panel.h"
#include "scroll_box.h"

ScrollBox::ScrollBox() {}

ScrollBox::ScrollBox(SDL_Rect rect, std::vector<std::unique_ptr<Panel>> p) {
  this->rectangle = rect;
  this->panels    = std::move(p);
}

void ScrollBox::addPanel(std::unique_ptr<Panel> panel) {
  this->panels.push_back(std::move(panel));
}

void ScrollBox::render() {
  for (auto& x : this->panels) {
    x->render();
  }
}
