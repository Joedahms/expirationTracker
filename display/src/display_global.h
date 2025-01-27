#ifndef DISPLAY_GLOBAL_H
#define DISPLAY_GLOBAL_H

#define MAIN_MENU  0
#define SCANNING   1
#define PAUSE_MENU 2
#define ITEM_LIST  3

#include <SDL2/SDL.h>

struct DisplayGlobal {
  SDL_Renderer* renderer;
  SDL_Window* window;
};

#endif
