#ifndef ELEMENT_H
#define ELEMENT_H

#include <SDL2/SDL.h>

class Element {
public:
  void centerVertical();
  void centerHorizontal();

protected:
  SDL_Rect rectangle;
};

#endif
