#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <glog/logging.h>
#include <iostream>
#include <string>

#include "element.h"
#include "text.h"

/**
 * @param displayGlobal Global variables
 * @param fontPath Path to where the font is stored.
 * @param content What characters the text represents.
 * @param fontSize Size of the font
 * @param color Color of the font
 * @param rectangle SDL rectangle defining the text
 */
Text::Text(struct DisplayGlobal displayGlobal,
           const char* fontPath,
           const char* content,
           int fontSize,
           SDL_Color color,
           SDL_Rect rectangle)
    : content(content), fontSize(fontSize), color(color) {
  this->displayGlobal = displayGlobal;
  this->font          = TTF_OpenFont(fontPath, this->fontSize);
  if (this->font == NULL) {
    LOG(FATAL) << "Text failed to open font";
  }

  this->rectangle          = rectangle;
  SDL_Surface* textSurface = TTF_RenderText_Solid(this->font, this->content, this->color);
  this->texture = SDL_CreateTextureFromSurface(this->displayGlobal.renderer, textSurface);
  SDL_FreeSurface(textSurface);

  SDL_QueryTexture(this->texture, NULL, NULL, &this->rectangle.w, &this->rectangle.h);
}

Text::~Text() { TTF_CloseFont(this->font); }

void Text::update() {}

/**
 * Input: None
 * Output: None
 */
void Text::render() const {
  SDL_RenderCopy(this->displayGlobal.renderer, this->texture, NULL, &this->rectangle);
}
