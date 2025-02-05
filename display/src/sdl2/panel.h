#ifndef PANEL_H
#define PANEL_H

#include <SDL2/SDL.h>
#include <memory>
#include <vector>

#include "button.h"
#include "display_global.h"
#include "element.h"
#include "text.h"

class Panel : public Element {
public:
  Panel(struct DisplayGlobal displayGlobal,
        SDL_Rect rect,
        std::vector<std::unique_ptr<Text>> t);
  Panel(struct DisplayGlobal displayGlobal,
        SDL_Rect rect,
        std::vector<std::unique_ptr<Text>> t,
        std::vector<std::unique_ptr<Button>> b);

  void updateElementPositions();
  void render() const override;

private:
  std::vector<std::unique_ptr<Text>> texts;
  std::vector<std::unique_ptr<Button>> buttons;
};

#endif
