#ifndef BUTTON_H
#define BUTTON_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <functional>
#include <memory>
#include <string>

#include "../display_global.h"
#include "composite_element.h"
#include "element.h"
#include "text.h"

/**
 * Clickable button containing centered text. Can be set to perform an action upon click.
 */
class Button : public CompositeElement {
public:
  Button(struct DisplayGlobal dg,
         const SDL_Rect& rectangle,
         const std::string& textContent,
         std::function<void()> callback);

  void updateSelf() override;
  void renderSelf() const override;
  void handleEventSelf(const SDL_Event& event) override;

private:
  std::function<void()> onClick;
  SDL_Color backgroundColor;
  SDL_Color defaultColor;
  SDL_Color hoveredColor;

  bool checkHovered(const int& mouseXPosition, const int& mouseYPosition);
};

#endif
