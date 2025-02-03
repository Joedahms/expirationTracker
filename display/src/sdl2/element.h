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

  template <typename T> bool checkCenterVertical(T centerWithin) {
    bool centered = false;
    if (this->rectangle.y == (centerWithin->h / 2 - this->rectangle.h / 2)) {
      centered = true;
    }
    return centered;
  }

  template <typename T> bool checkCenterHorizontal(T centerWithin) {
    bool centered = false;
    if (this->rectangle.x == (centerWithin->w / 2 - this->rectangle.w / 2)) {
      centered = true;
    }
    return centered;
  }

  SDL_Rect getRectangle();
  void setRectangle(SDL_Rect rectangle);

protected:
  SDL_Rect rectangle;
};

template <> void Element::centerVertical<SDL_Rect>(SDL_Rect centerWithin);
template <> void Element::centerHorizontal<SDL_Rect>(SDL_Rect centerWithin);

template <> bool Element::checkCenterVertical<SDL_Rect>(SDL_Rect centerWithin);
template <> bool Element::checkCenterHorizontal<SDL_Rect>(SDL_Rect centerWithin);

#endif
