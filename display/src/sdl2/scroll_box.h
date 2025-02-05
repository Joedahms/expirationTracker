#ifndef SCROLL_BOX_H
#define SCROLL_BOX_H

#include <SDL2/SDL.h>
#include <memory>
#include <vector>

#include "../../../food_item.h"
#include "display_global.h"
#include "element.h"
#include "panel.h"

class ScrollBox : public Element {
public:
  ScrollBox(struct DisplayGlobal dg);
  void setPanelHeight(int panelHeight);
  void updatePanels(std::vector<FoodItem> allFoodItems);
  void addPanel(std::unique_ptr<Panel> panel);
  void render() const override;

private:
  struct DisplayGlobal displayGlobal;
  std::vector<std::unique_ptr<Panel>> panels;
  int panelHeight;
};

#endif
