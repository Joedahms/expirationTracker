#ifndef TEXT_H
#define TEXT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

#include "game_global.h"
#include "rectangle.h"

/**
 * @class Text
 *
 * Represents a string of characters that can be rendered.
 */
class Text {
public:
  Text(struct GameGlobal, const char*, const char*, int, SDL_Color, SDL_Rect);

  // Center text horizontally within a type
  template <typename T> void centerHorizontal(T centerWithin) {
    this->rectangle = centerRectangleHorizontal(centerWithin, this->rectangle);
  }

  // Center text vertically within a type
  template <typename T> void centerVertical(T centerWithin) {
    this->rectangle = centerRectangleVertical(centerWithin, this->rectangle);
  }

  void render();

private:
  struct GameGlobal gameGlobal;
  TTF_Font* font;
  const char* content;
  int fontSize;
  SDL_Color color;
  SDL_Texture* texture;
  SDL_Rect rectangle;
};

#endif
