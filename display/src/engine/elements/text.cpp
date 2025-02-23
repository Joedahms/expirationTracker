#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <glog/logging.h>
#include <iostream>
#include <string>

#include "../sdl_debug.h"
#include "element.h"
#include "text.h"

Text::Text() {}

Text::Text(struct DisplayGlobal displayGlobal,
           const std::string& fontPath,
           const std::string& content,
           int fontSize,
           SDL_Color color,
           SDL_Rect boundaryRectangle)
    : content(content), fontSize(fontSize), color(color) {
  this->displayGlobal = displayGlobal;
  this->font          = TTF_OpenFont(fontPath.c_str(), this->fontSize);
  if (this->font == NULL) {
    LOG(FATAL) << "Text failed to open font";
  }

  this->boundaryRectangle = boundaryRectangle;
  SDL_Surface* textSurface =
      TTF_RenderText_Solid(this->font, this->content.c_str(), this->color);
  this->texture = SDL_CreateTextureFromSurface(this->displayGlobal.renderer, textSurface);
  SDL_FreeSurface(textSurface);

  SDL_QueryTexture(this->texture, NULL, NULL, &this->boundaryRectangle.w,
                   &this->boundaryRectangle.h);
}

/**
 * @param displayGlobal Global variables
 * @param fontPath Path to where the font is stored.
 * @param content What characters the text represents.
 * @param fontSize Size of the font
 * @param color Color of the font
 * @param boundaryRectangle SDL boundaryRectangle defining the text
 */
Text::Text(struct DisplayGlobal displayGlobal,
           const std::string& fontPath,
           const std::string& content,
           int fontSize,
           SDL_Color color,
           SDL_Rect boundaryRectangle,
           SDL_Point positionRelativeToParent)
    : content(content), fontSize(fontSize), color(color) {
  this->displayGlobal            = displayGlobal;
  this->positionRelativeToParent = positionRelativeToParent;
  this->font                     = TTF_OpenFont(fontPath.c_str(), this->fontSize);
  if (this->font == NULL) {
    LOG(FATAL) << "Text failed to open font";
  }

  this->boundaryRectangle = boundaryRectangle;
  SDL_Surface* textSurface =
      TTF_RenderText_Solid(this->font, this->content.c_str(), this->color);
  this->texture = SDL_CreateTextureFromSurface(this->displayGlobal.renderer, textSurface);
  SDL_FreeSurface(textSurface);

  SDL_QueryTexture(this->texture, NULL, NULL, &this->boundaryRectangle.w,
                   &this->boundaryRectangle.h);
}

Text::~Text() { TTF_CloseFont(this->font); }

void Text::setContent(const std::string& content) {
  this->content = content;

  SDL_Surface* textSurface =
      TTF_RenderText_Solid(this->font, this->content.c_str(), this->color);
  this->texture = SDL_CreateTextureFromSurface(this->displayGlobal.renderer, textSurface);
  SDL_FreeSurface(textSurface);
}

void Text::render() const {
  SDL_RenderCopy(this->displayGlobal.renderer, this->texture, NULL,
                 &this->boundaryRectangle);
}

void Text::handleEvent(const SDL_Event& event) {}
