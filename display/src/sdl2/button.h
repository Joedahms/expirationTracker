#ifndef BUTTON_H
#define BUTTON_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <memory>
#include <string>

#include "display_global.h"
#include "element.h"
#include "text.h"

class Button : public Element {
public:
  Button(struct DisplayGlobal displayGlobal, struct SDL_Rect, const std::string&);
  bool checkHovered(int mouseXPosition, int mouseYPosition);
  void render();

private:
  struct DisplayGlobal displayGlobal;
  std::unique_ptr<Text> text; // Text within the button
  SDL_Color backgroundColor;  // Current color
  SDL_Color defaultColor;     // Color when not hovered
  SDL_Color hoveredColor;     // Color when hovered
};

#endif
