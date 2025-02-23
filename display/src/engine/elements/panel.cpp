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
 * Construct a panel with a boundaryRectangle and some text objects.
 *
 * @param displayGlobal
 * @param id ID of the object to be displayed within the panel. Could correspond to an ID
 * of an item in a database.
 * @param rect SDL boundaryRectangle defining the position and size of the panel.
 * @param t A vector of texts to be displayed within the panel.
 */
Panel::Panel(struct DisplayGlobal displayGlobal,
             int id,
             SDL_Rect boundaryRectangle,
             SDL_Point positionRelativeToParent,
             std::vector<std::unique_ptr<Text>> t)
    : id(id) {
  this->boundaryRectangle        = boundaryRectangle;
  this->positionRelativeToParent = positionRelativeToParent;
  // Add text
  for (auto& currText : t) {
    addElement(std::move(currText));
  }

  // Add quantity setting
  std::unique_ptr<NumberSetting> itemQuantity =
      std::make_unique<NumberSetting>(displayGlobal, id);
  addElement(std::move(itemQuantity));

  this->displayGlobal = displayGlobal;
}

Panel::Panel(struct DisplayGlobal displayGlobal,
             int id,
             SDL_Point positionRelativeToParent)
    : id(id) {
  this->positionRelativeToParent = positionRelativeToParent;
  this->displayGlobal            = displayGlobal;
}

/**
 * Add some text to a panel. Whatever text is added first will be displayed on the left
 * and any text added after that will be displayed moving right.
 *
 * @param fontPath File path to the font of the text
 * @param content The actual string of text to add
 * @param fontSize How big the text font should be
 * @param color RGB value for the text color
 * @return None
 */
void Panel::addText(const std::string& fontPath,
                    const std::string& content,
                    const int& fontSize,
                    const SDL_Color& color,
                    const SDL_Point& relativePosition) {
  std::unique_ptr<Text> text =
      std::make_unique<Text>(this->displayGlobal, fontPath, content, fontSize, color,
                             SDL_Rect{0, 0, 0, 0}, relativePosition);
  addElement(std::move(text));
}

/**
 * General way to add a food item to a panel.
 *
 * @param foodItem The food item to add to the panel
 * @return None
 */
void Panel::addFoodItem(const FoodItem& foodItem, const SDL_Point& relativePosition) {
  addFoodItemName(foodItem, relativePosition);
  addFoodItemExpirationDate(foodItem, relativePosition);

  std::unique_ptr<NumberSetting> itemQuantity =
      std::make_unique<NumberSetting>(this->displayGlobal, this->id);
  addElement(std::move(itemQuantity));
}

/**
 * Currently this function only updates the text objects, it will need to be expanded to
 * update buttons in the future when they are added. It ensures that all of the text
 * objects are aligned as desired within the panel.
 *
 * @param None
 * @return None
 */
void Panel::updateSelf() {
  // Align all children right next to each other
  for (int i = 0; i < this->children.size(); i++) {
    SDL_Point childRelativePosition = this->children[i]->getPositionRelativeToParent();
    childRelativePosition.y         = this->positionRelativeToParent.y;
    if (i == 0) {
      childRelativePosition.x = 0;
    }
    else {
      SDL_Point leftRelativePosition =
          this->children[i - 1]->getPositionRelativeToParent();
      SDL_Rect leftBoundaryRectangle = this->children[i - 1]->getBoundaryRectangle();
      childRelativePosition.x        = leftRelativePosition.x + leftBoundaryRectangle.w;
    }
    this->children[i]->setPositionRelativeToParent(childRelativePosition);
  }
}

void Panel::renderSelf() const {}
void Panel::handleEventSelf(const SDL_Event& event) {}

/**
 * Add the name of a food item to a panel.
 *
 * @param foodItem The food item to take the name from
 * @return None
 */
void Panel::addFoodItemName(const FoodItem& foodItem, const SDL_Point& relativePosition) {
  std::string fontPath = this->displayGlobal.futuramFontPath;
  int fontSize         = 24;
  SDL_Color textColor  = {0, 255, 0, 255}; // Green

  addText(fontPath, foodItem.name, fontSize, textColor, relativePosition);
}

/**
 * Add the expiration date of a food item to a panel. Expires: then mm/dd/yyyy
 *
 * @param foodItem The food item to take the expiration date from
 * @return None
 */
void Panel::addFoodItemExpirationDate(const FoodItem& foodItem,
                                      const SDL_Point& relativePosition) {
  std::string fontPath = this->displayGlobal.futuramFontPath;
  int fontSize         = 24;
  SDL_Color textColor  = {0, 255, 0, 255}; // Green

  // Expires
  addText(fontPath, " Expires: ", fontSize, textColor, relativePosition);

  // Month
  std::string expirationDateMonth =
      std::to_string(static_cast<unsigned>(foodItem.expirationDate.month()));
  addText(fontPath, expirationDateMonth, fontSize, textColor, relativePosition);

  // Slash
  addText(fontPath, "/", fontSize, textColor, relativePosition);

  // Date
  std::string expirationDateDay =
      std::to_string(static_cast<unsigned>(foodItem.expirationDate.day()));
  addText(fontPath, expirationDateDay, fontSize, textColor, relativePosition);

  // Slash
  addText(fontPath, "/", fontSize, textColor, relativePosition);

  // Year
  std::string expirationDateYear =
      std::to_string(static_cast<int>(foodItem.expirationDate.year()));
  addText(fontPath, expirationDateYear, fontSize, textColor, relativePosition);
}
