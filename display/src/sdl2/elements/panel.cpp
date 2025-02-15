#include <SDL2/SDL.h>
#include <iostream>
#include <memory>
#include <vector>

#include "../display_global.h"
#include "../sdl_debug.h"
#include "button.h"
#include "element.h"
#include "panel.h"
#include "text.h"

/**
 * Construct a panel with just a rectangle.
 *
 * @param rect SDL rectangle bounding the panel
 * @param t Vector of text objects to display within the panel
 */
Panel::Panel(struct DisplayGlobal displayGlobal, int id)
    : itemQuantity(displayGlobal, id) {
  this->displayGlobal = displayGlobal;
}

/**
 * Construct a panel with just a rectangle.
 *
 * @param rect SDL rectangle bounding the panel
 * @param t Vector of text objects to display within the panel
 */
Panel::Panel(struct DisplayGlobal displayGlobal, int id, SDL_Rect rect)
    : itemQuantity(displayGlobal, id) {
  this->displayGlobal = displayGlobal;
  this->rectangle     = rect;
}

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
    : itemQuantity(displayGlobal, id) {
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
    : itemQuantity(displayGlobal, id) {
  this->displayGlobal = displayGlobal;
  this->rectangle     = rect;
  this->texts         = std::move(t);
  this->buttons       = std::move(b);
}

void Panel::addFoodItemName(const FoodItem& foodItem) {
  SDL_Color textColor = {0, 255, 0, 255}; // Green
  SDL_Rect rect       = {0, 0, 0, 0};

  const char* foodItemName = foodItem.name.c_str();
  std::unique_ptr<Text> name =
      std::make_unique<Text>(this->displayGlobal, this->displayGlobal.futuramFontPath,
                             foodItemName, 24, textColor, rect);

  this->texts.push_back(std::move(name));
}

void Panel::addFoodItemExpirationDate(const FoodItem& foodItem) {
  SDL_Color textColor = {0, 255, 0, 255}; // Green
  SDL_Rect rect       = {0, 0, 0, 0};

  // Expires
  std::unique_ptr<Text> expiration =
      std::make_unique<Text>(this->displayGlobal, this->displayGlobal.futuramFontPath,
                             " Expires: ", 24, textColor, rect);
  this->texts.push_back(std::move(expiration));

  // Month
  std::string expirationDateMonth =
      std::to_string(static_cast<unsigned>(foodItem.expirationDate.month()));
  std::unique_ptr<Text> month =
      std::make_unique<Text>(this->displayGlobal, this->displayGlobal.futuramFontPath,
                             expirationDateMonth.c_str(), 24, textColor, rect);
  this->texts.push_back(std::move(month));

  // Slash
  std::unique_ptr<Text> slash = std::make_unique<Text>(
      this->displayGlobal, this->displayGlobal.futuramFontPath, "/", 24, textColor, rect);
  this->texts.push_back(std::move(slash));

  // Date
  std::string expirationDateDay =
      std::to_string(static_cast<unsigned>(foodItem.expirationDate.day()));
  std::unique_ptr<Text> day =
      std::make_unique<Text>(this->displayGlobal, this->displayGlobal.futuramFontPath,
                             expirationDateDay.c_str(), 24, textColor, rect);
  this->texts.push_back(std::move(day));

  // Slash
  std::unique_ptr<Text> slash2 = std::make_unique<Text>(
      this->displayGlobal, this->displayGlobal.futuramFontPath, "/", 24, textColor, rect);
  this->texts.push_back(std::move(slash2));

  // Year
  std::string expirationDateYear =
      std::to_string(static_cast<int>(foodItem.expirationDate.year()));
  std::unique_ptr<Text> year =
      std::make_unique<Text>(this->displayGlobal, this->displayGlobal.futuramFontPath,
                             expirationDateYear.c_str(), 24, textColor, rect);
  this->texts.push_back(std::move(year));
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
      // printRect(leftRectangle);
      textRectangle.x = leftRectangle.x + leftRectangle.w;
    }
    this->texts[i]->setRectangle(textRectangle);
  }
  SDL_Rect itemQuantityRect = this->itemQuantity.getRectangle();
  itemQuantityRect.y        = this->texts.back()->getRectangle().y;
  itemQuantityRect.x =
      this->texts.back()->getRectangle().x + this->texts.back()->getRectangle().w;
  this->itemQuantity.setRectangle(itemQuantityRect);
  this->itemQuantity.update();
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
  this->itemQuantity.render();
}
