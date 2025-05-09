#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>

#include "../sdl_debug.h"
#include "element.h"
#include "text.h"

Text::Text(const struct DisplayGlobal& displayGlobal,
           const std::string& logFile,
           const SDL_Rect boundaryRectangle,
           const std::string& fontPath,
           const std::string& content,
           const int fontSize,
           const SDL_Color color)
    : Element(displayGlobal, logFile, boundaryRectangle), content(content),
      fontSize(fontSize), color(color) {
  this->font = TTF_OpenFont(fontPath.c_str(), this->fontSize);
  if (this->font == NULL) {
    std::cerr << "Text failed to open font";
    exit(1);
  }

  SDL_Surface* textSurface =
      TTF_RenderText_Solid(this->font, this->content.c_str(), this->color);
  this->texture = SDL_CreateTextureFromSurface(this->displayGlobal.renderer, textSurface);
  SDL_FreeSurface(textSurface);

  // Set width and height
  SDL_QueryTexture(this->texture, NULL, NULL, &this->boundaryRectangle.w,
                   &this->boundaryRectangle.h);
}

Text::~Text() {
  SDL_DestroyTexture(this->texture);
  TTF_CloseFont(this->font);
}

void Text::setContent(const std::string& content) {
  this->content = content;

  if (this->texture) {
    SDL_DestroyTexture(this->texture);
    this->texture = nullptr;
  }

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
