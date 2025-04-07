#ifndef SCROLL_BOX_H
#define SCROLL_BOX_H

#include <SDL2/SDL.h>
#include <chrono>
#include <memory>
#include <vector>

#include "../../../../food_item.h"
#include "../display_global.h"
#include "panel.h"
#include "sort_method.h"

/**
 * An element that contains a column of panels and allows the user to scroll through them
 * with the mousewheel.
 *
 * @see Panel
 */
class ScrollBox : public CompositeElement {
public:
  ScrollBox(struct DisplayGlobal displayGlobal,
            const SDL_Rect& boundaryRectangle,
            const std::string& logFile);

  void setPanelHeight(int panelHeight);

  void updateSelf() override;
  void renderSelf() const override;
  void handleEventSelf(const SDL_Event& event);

  void setSortMethod(SortMethod sortMethod);

private:
  void refreshPanels();
  void scroll();

  void handleMouseDown();
  void handleMouseMotion(const SDL_Event& event);
  void handleMouseUp();

  int panelHeight;
  int topPanelPosition = 0;
  int scrollAmount     = 0;

  SortMethod sortMethod = SortMethod::LOW_TO_HIGH;

  SDL_Point previousMotion = {0, 0};
};

#endif
