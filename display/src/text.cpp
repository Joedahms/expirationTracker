#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

#include "rectangle.h"
#include "text.h"

/**
 * @constructor Text
 *
 * @param gameGlobal
 * @param fontPath - Path to where the font is stored.
 * @param content - What characters the text represents.
 * @param fontSize
 * @param color
 * @param rectangle
 */
Text::Text(struct GameGlobal gameGlobal,
           const char* fontPath,
           const char* content,
           int fontSize,
           SDL_Color color,
           SDL_Rect rectangle) {
  this->gameGlobal = gameGlobal;

  this->content  = content;
  this->fontSize = fontSize;

  this->font = TTF_OpenFont(fontPath, this->fontSize);
  if (this->font == NULL) {
    //    writeToLogFile(this->gameGlobal.logFile, TTF_GetError());
  }

  this->color     = color;
  this->rectangle = rectangle;

  SDL_Surface* textSurface = TTF_RenderText_Solid(this->font, this->content, this->color);
  this->texture = SDL_CreateTextureFromSurface(this->gameGlobal.renderer, textSurface);
  SDL_FreeSurface(textSurface);

  SDL_QueryTexture(this->texture, NULL, NULL, &this->rectangle.w, &this->rectangle.h);
}

/**
 * @function render
 *
 * @param none
 * @output none
 */
void Text::render() {
  SDL_RenderCopy(this->gameGlobal.renderer, this->texture, NULL, &this->rectangle);
}
