#ifndef SCROLL_BOX_H
#define SCROLL_BOX_H

#include <SDL2/SDL.h>
#include <memory>
#include <vector>

#include "element.h"
#include "panel.h"

class ScrollBox : public Element {
public:
  ScrollBox(SDL_Rect rect, std::vector<std::unique_ptr<Panel>> p);
  void render();

private:
  std::vector<std::unique_ptr<Panel>> panels;
};

#endif
