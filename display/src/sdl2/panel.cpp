#include <SDL2/SDL.h>
#include <iostream>
#include <memory>
#include <vector>

#include "button.h"
#include "element.h"
#include "panel.h"
#include "text.h"

Panel::Panel(SDL_Rect rect, std::vector<std::unique_ptr<Text>> t) {
  this->rectangle = rect;
  this->texts     = std::move(t);
}

Panel::Panel(SDL_Rect rect,
             std::vector<std::unique_ptr<Text>> t,
             std::vector<std::unique_ptr<Button>> b) {
  this->rectangle = rect;
  this->texts     = std::move(t);
  this->buttons   = std::move(b);
}

void Panel::updateElements() {
  for (auto& x : this->texts) {
    SDL_Rect textRectangle = x->getRectangle();
    textRectangle.x        = this->rectangle.x;
    textRectangle.y        = this->rectangle.y;
    x->setRectangle(textRectangle);
  }
}

void Panel::render() {
  for (auto& x : this->texts) {
    x->render();
  }
}
