#ifndef SCROLL_BOX_H
#define SCROLL_BOX_H

#include <SDL2/SDL.h>
#include <chrono>
#include <memory>
#include <vector>

#include "../../../../food_item.h"
#include "../display_global.h"
#include "element.h"
#include "panel.h"

class ScrollBox : public Element {
public:
  ScrollBox(struct DisplayGlobal displayGlobal);
  void refreshPanels();
  void setPanelHeight(int panelHeight);
  void update() override;
  void addPanel(std::unique_ptr<Panel> panel, SDL_Rect containingRectangle);
  void scrollUp(const SDL_Point* mousePosition);
  void scrollDown(const SDL_Point* mousePosition);
  void render() const override;

private:
  std::chrono::steady_clock::time_point previousUpdate;
  std::chrono::steady_clock::time_point currentUpdate;
  std::vector<std::unique_ptr<Panel>> panels;
  int panelHeight;
  int topPanelPosition = 0;
};

#endif
