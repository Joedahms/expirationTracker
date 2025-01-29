#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

#include "rectangle.h"
#include "text.h"

/**
 * - displayGlobal - Global variables
 * - fontPath - Path to where the font is stored.
 * - content - What characters the text represents.
 * - fontSize - Size of the font
 * - color - Color of the font
 * - rectangle - SDL rectangle defining the text
 */
Text::Text(struct DisplayGlobal displayGlobal,
           const char* fontPath,
           const char* content,
           int fontSize,
           SDL_Color color,
           SDL_Rect rectangle) {
  this->displayGlobal = displayGlobal;

  this->content  = content;
  this->fontSize = fontSize;

  this->font = TTF_OpenFont(fontPath, this->fontSize);
  if (this->font == NULL) {
    //    writeToLogFile(this->displayGlobal.logFile, TTF_GetError());
  }

  this->color     = color;
  this->rectangle = rectangle;

  SDL_Surface* textSurface = TTF_RenderText_Solid(this->font, this->content, this->color);
  this->texture = SDL_CreateTextureFromSurface(this->displayGlobal.renderer, textSurface);
  SDL_FreeSurface(textSurface);

  SDL_QueryTexture(this->texture, NULL, NULL, &this->rectangle.w, &this->rectangle.h);
}

/**
 * Input: None
 * Output: None
 */
void Text::render() {
  SDL_RenderCopy(this->displayGlobal.renderer, this->texture, NULL, &this->rectangle);
}
