#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <SDL2/SDL.h>

// Center a rectangle horizontally within another type
// Can center within a type with a w(width) field
template <typename T>
SDL_Rect centerRectangleHorizontal(T centerWithin, const SDL_Rect rectangle) {
  SDL_Rect centeredRectangle = rectangle;
  centeredRectangle.x = (centerWithin->w / 2 - rectangle.w / 2);
  return centeredRectangle;
}

// Center a rectangle horizontally within another type
// Can center within a type with a h(height) field
template <typename T>
SDL_Rect centerRectangleVertical(T centerWithin, const SDL_Rect rectangle) {
  SDL_Rect centeredRectangle = rectangle;
  centeredRectangle.y = (centerWithin->h / 2 - rectangle.h / 2);
  return centeredRectangle;
}

// Center a rectangle horizonatally within a rectangle
template <>
inline SDL_Rect centerRectangleHorizontal<SDL_Rect*>(SDL_Rect* centerWithin, const SDL_Rect rectangle) {
  SDL_Rect centeredRectangle = rectangle;
  centeredRectangle.x = (centerWithin->w / 2 - rectangle.w / 2) + centerWithin->x;
  return centeredRectangle;
}

// Center a rectangle vertically within a rectangle
template <>
inline SDL_Rect centerRectangleVertical<SDL_Rect*>(SDL_Rect* centerWithin, const SDL_Rect rectangle) {
  SDL_Rect centeredRectangle = rectangle;
  centeredRectangle.y = (centerWithin->h / 2 - rectangle.h / 2) + centerWithin->y;
  return centeredRectangle;
}

#endif
