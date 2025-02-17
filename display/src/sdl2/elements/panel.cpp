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
 * Most basic way to construct a panel. Only need the id of the object to be displayed
 * within the panel.
 *
 * @param displayGlobal
 * @param id ID of the object to be displayed within the panel. Could correspond to an ID
 * of an item in a database.
 */
Panel::Panel(struct DisplayGlobal displayGlobal, int id)
    : itemQuantity(displayGlobal, id) {
  this->displayGlobal = displayGlobal;
}

/**
 * Construct a panel with an id and a rectangle. Used to define an explicit rectangle for
 * the panel.
 *
 * @param displayGlobal
 * @param id ID of the object to be displayed within the panel. Could correspond to an ID
 * of an item in a database.
 * @param rect SDL rectangle defining the position and size of the panel.
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
 * @param displayGlobal
 * @param id ID of the object to be displayed within the panel. Could correspond to an ID
 * of an item in a database.
 * @param rect SDL rectangle defining the position and size of the panel.
 * @param t A vector of texts to be displayed within the panel.
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

void Panel::handleMouseButtonDown(const SDL_Point& mousePosition) {
  this->itemQuantity.handleMouseButtonDown(mousePosition);
}

/**
 * Add some text to a panel. Whatever text is added first will be displayed on the left
 * and any text added after that will be displayed moving right.
 *
 * @param fontPath File path to the font of the text
 * @param content The actual string of text to add
 * @param fontSize How big the text font should be
 * @param color RGB value for the text color
 */
void Panel::addText(const std::string& fontPath,
                    const std::string& content,
                    const int& fontSize,
                    const SDL_Color& color) {
  std::unique_ptr<Text> text = std::make_unique<Text>(
      this->displayGlobal, fontPath, content, fontSize, color, SDL_Rect{0, 0, 0, 0});
  this->texts.push_back(std::move(text));
}

/**
 * General way to add a food item to a panel.
 *
 * @param foodItem The food item to add to the panel
 * @return None
 */
void Panel::addFoodItem(const FoodItem& foodItem) {
  addFoodItemName(foodItem);
  addFoodItemExpirationDate(foodItem);
}

/**
 * Add the name of a food item to a panel.
 *
 * @param foodItem The food item to take the name from
 * @return None
 */
void Panel::addFoodItemName(const FoodItem& foodItem) {
  std::string fontPath = this->displayGlobal.futuramFontPath;
  int fontSize         = 24;
  SDL_Color textColor  = {0, 255, 0, 255}; // Green

  addText(fontPath, foodItem.name, fontSize, textColor);
}

/**
 * Add the expiration date of a food item to a panel. Expires: then mm/dd/yyyy
 *
 * @param foodItem The food item to take the expiration date from
 * @return None
 */
void Panel::addFoodItemExpirationDate(const FoodItem& foodItem) {
  std::string fontPath = this->displayGlobal.futuramFontPath;
  int fontSize         = 24;
  SDL_Color textColor  = {0, 255, 0, 255}; // Green

  // Expires
  addText(fontPath, " Expires: ", fontSize, textColor);

  // Month
  std::string expirationDateMonth =
      std::to_string(static_cast<unsigned>(foodItem.expirationDate.month()));
  addText(fontPath, expirationDateMonth, fontSize, textColor);

  // Slash
  addText(fontPath, "/", fontSize, textColor);

  // Date
  std::string expirationDateDay =
      std::to_string(static_cast<unsigned>(foodItem.expirationDate.day()));
  addText(fontPath, expirationDateDay, fontSize, textColor);

  // Slash
  addText(fontPath, "/", fontSize, textColor);

  // Year
  std::string expirationDateYear =
      std::to_string(static_cast<int>(foodItem.expirationDate.year()));
  addText(fontPath, expirationDateYear, fontSize, textColor);
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

  // Add quantity
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
  // Border
  if (this->hasBorder) {
    renderBorder();
  }

  // Text
  for (auto& x : this->texts) {
    x->render();
  }

  // Quantity
  this->itemQuantity.render();
}
