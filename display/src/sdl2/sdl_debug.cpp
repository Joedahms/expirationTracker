#include <SDL2/SDL.h>
#include <iostream>
#include <string>

#include "sdl_debug.h"

void printRect(const SDL_Rect& rectangle, const std::string& name) {
  std::cout << "SDL Rectangle " << name << ": " << std::endl;
  std::cout << "X Position: " << rectangle.x << std::endl;
  std::cout << "Y Position: " << rectangle.y << std::endl;
  std::cout << "Width: " << rectangle.w << std::endl;
  std::cout << "Height: " << rectangle.h << std::endl << std::endl;
}
