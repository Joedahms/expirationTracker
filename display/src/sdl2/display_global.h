#ifndef DISPLAY_GLOBAL_H
#define DISPLAY_GLOBAL_H

#include <SDL2/SDL.h>

struct DisplayGlobal {
  SDL_Renderer* renderer;
  SDL_Window* window;
  const char* futuramFontPath = "../display/fonts/16020_FUTURAM.ttf";
};

#endif
