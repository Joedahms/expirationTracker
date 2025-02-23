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

/**
 * An element that contains a column of panels and allows the user to scroll through them
 * with the mousewheel.
 *
 * @see Panel
 */
class ScrollBox : public CompositeElement {
public:
  ScrollBox(struct DisplayGlobal displayGlobal, const SDL_Rect& boundaryRectangle);

  void refreshPanels();
  void setPanelHeight(int panelHeight);
  void addPanel(std::unique_ptr<Panel> panel, SDL_Rect containingRectangle);

  // void scrollUp(const SDL_Point* mousePosition);
  // void scrollDown(const SDL_Point* mousePosition);

  void updateSelf() override;
  void renderSelf() const override;
  void handleEventSelf(const SDL_Event& event);

private:
  std::chrono::steady_clock::time_point previousUpdate;
  std::chrono::steady_clock::time_point currentUpdate;
  int panelHeight;
  int topPanelPosition = 0;
};

#endif
