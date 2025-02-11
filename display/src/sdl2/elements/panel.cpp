#include <SDL2/SDL.h>
#include <iostream>
#include <memory>
#include <vector>

#include "../display_global.h"
#include "button.h"
#include "element.h"
#include "panel.h"
#include "text.h"

/**
 * Construct a panel with just a rectangle and some text objects. Currently a temporary
 * function as buttons haven't been integreted into panels yet.
 *
 * @param rect SDL rectangle bounding the panel
 * @param t Vector of text objects to display within the panel
 */
Panel::Panel(struct DisplayGlobal displayGlobal,
             int id,
             SDL_Rect rect,
             std::vector<std::unique_ptr<Text>> t)
    : foodItemId(id) {
  this->displayGlobal = displayGlobal;
  this->rectangle     = rect;
  this->texts         = std::move(t);
}

/**
 * Construct a panel with a rectangle, some text, and some buttons. This is not currently
 * used as no buttons in panels yet.
 *
 * @param rect SDL rectangle bounding the panel
 * @param t Vector of text objects to display within the panel
 * @param b Vector of button objects to display within the panel
 */
Panel::Panel(struct DisplayGlobal displayGlobal,
             int id,
             SDL_Rect rect,
             std::vector<std::unique_ptr<Text>> t,
             std::vector<std::unique_ptr<Button>> b)
    : foodItemId(id) {
  this->displayGlobal = displayGlobal;
  this->rectangle     = rect;
  this->texts         = std::move(t);
  this->buttons       = std::move(b);
}

/**
 * Currently this function only updates the text objects, it will need to be expanded to
 * update buttons in the future when they are added. It ensures that all of the text
 * objects are aligned as desired within the panel.
 *
 * @param None
 * @return None
 */
void Panel::update() {
  // Align all text objects right next to each other
  for (int i = 0; i < this->texts.size(); i++) {
    SDL_Rect textRectangle = this->texts[i]->getRectangle();
    textRectangle.y        = this->rectangle.y;
    if (i == 0) {
      textRectangle.x = 0;
    }
    else {
      SDL_Rect leftRectangle = this->texts[i - 1]->getRectangle();
      textRectangle.x        = leftRectangle.x + leftRectangle.w;
    }
    this->texts[i]->setRectangle(textRectangle);
  }
  this->quantity.update();
}

/**
 * Render all elements within the panel.
 *
 * @param None
 * @return None
 */
void Panel::render() const {
  if (this->hasBorder) {
    renderBorder();
  }
  for (auto& x : this->texts) {
    x->render();
  }
  this->quantity.render();
}
