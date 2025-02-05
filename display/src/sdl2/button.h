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
  Button(struct DisplayGlobal dg,
         const SDL_Rect& rectangle,
         const std::string& textContent);
  Button(struct DisplayGlobal dg,
         const SDL_Rect& rectangle,
         const std::string& textContent,
         const int& clickRet);
  bool checkHovered(const int& mouseXPosition, const int& mouseYPosition);
  int getClickReturn() const;
  void update();
  void render() const override;

private:
  struct DisplayGlobal displayGlobal;
  std::unique_ptr<Text> text; // Text within the button
  SDL_Color backgroundColor;  // Current color
  SDL_Color defaultColor;     // Color when not hovered
  SDL_Color hoveredColor;     // Color when hovered
  int clickReturn = -1;
};

#endif
