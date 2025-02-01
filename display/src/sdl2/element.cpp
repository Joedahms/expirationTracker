#include <SDL2/SDL.h>

#include "element.h"

template <> void Element::centerVertical<SDL_Rect>(SDL_Rect centerWithin) {
  this->rectangle.y = (centerWithin.h / 2 - this->rectangle.h / 2) + centerWithin.y;
}

template <> void Element::centerHorizontal<SDL_Rect>(SDL_Rect centerWithin) {
  this->rectangle.x = (centerWithin.w / 2 - this->rectangle.w / 2) + centerWithin.x;
}
