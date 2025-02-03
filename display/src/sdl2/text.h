#ifndef TEXT_H
#define TEXT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

#include "display_global.h"
#include "element.h"

/**
 * Represents a string of characters that can be rendered.
 */
class Text : public Element {
public:
  Text(struct DisplayGlobal displayGlobal,
       const char* fontPath,
       const char* content,
       int fontSize,
       SDL_Color color,
       SDL_Rect rectangle);
  void render();

private:
  struct DisplayGlobal displayGlobal;
  TTF_Font* font;
  const char* content;
  int fontSize;
  SDL_Color color;
  SDL_Texture* texture;
};

#endif
