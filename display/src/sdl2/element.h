#ifndef ELEMENT_H
#define ELEMENT_H

#include <SDL2/SDL.h>

class Element {
public:
  template <typename T> void centerVertical(T centerWithin) {
    this->rectangle.y = (centerWithin->h / 2 - this->rectangle.h / 2);
  }

  template <typename T> void centerHorizontal(T centerWithin) {
    this->rectangle.x = (centerWithin->w / 2 - this->rectangle.w / 2);
  }

protected:
  SDL_Rect rectangle;
};

template <> void Element::centerVertical<SDL_Rect>(SDL_Rect centerWithin);
template <> void Element::centerHorizontal<SDL_Rect>(SDL_Rect centerWithin);

#endif
