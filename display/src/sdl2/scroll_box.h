#ifndef SCROLL_BOX_H
#define SCROLL_BOX_H

#include <SDL2/SDL.h>
#include <memory>
#include <vector>

#include "element.h"
#include "panel.h"

class ScrollBox : public Element {
public:
  void setPanelHeight(int panelHeight);
  void addPanel(std::unique_ptr<Panel> panel);
  void clearPanels();
  void render();

private:
  std::vector<std::unique_ptr<Panel>> panels;
  int panelHeight;
};

#endif
