#ifndef BUTTON_H
#define BUTTON_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

#include "game_global.h"
#include "text.h"

class Button {
public:
  Button(struct GameGlobal gameGlobal, struct SDL_Rect, const std::string&);
  bool checkHovered(int mouseXPosition, int mouseYPosition);
  void render();

private:
  struct GameGlobal gameGlobal;
  std::unique_ptr<Text> text;   // Text within the button
  SDL_Rect backgroundRectangle; // Rectangle defining where to render the button to
  SDL_Color backgroundColor;    // Current color
  SDL_Color defaultColor;       // Color when not hovered
  SDL_Color hoveredColor;       // Color when hovered
};

#endif
