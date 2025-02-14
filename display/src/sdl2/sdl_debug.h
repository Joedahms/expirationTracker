#ifndef SDL_DEBUG
#define SDL_DEBUG

#include <SDL2/SDL.h>
#include <string>

void printRect(const SDL_Rect& rectangle, const std::string& name);

#endif
