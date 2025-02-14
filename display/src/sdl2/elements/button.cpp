#include <SDL2/SDL.h>
#include <iostream>
#include <memory>

#include "../display_global.h"
#include "button.h"
#include "element.h"

/**
 * Set basic properties and add/position the text within the button. Most basic button
 * constructor.
 *
 * @param displayGlobal Global display variables
 * @param rectangle Rectangle to render the button with
 * @param text The text to print in the middle of the button
 */
Button::Button(struct DisplayGlobal displayGlobal,
               const SDL_Rect& rectangle,
               const std::string& textContent) {
  this->displayGlobal = displayGlobal;
  this->rectangle     = rectangle;

  this->backgroundColor = {255, 0, 0, 255}; // Red
  this->hoveredColor    = {0, 255, 0, 255}; // Green
  this->defaultColor    = {255, 0, 0, 255}; // Red

  SDL_Color textColor = {255, 255, 0, 255}; // Yellow
  this->text =
      std::make_unique<Text>(this->displayGlobal, "../display/fonts/16020_FUTURAM.ttf",
                             textContent.c_str(), 24, textColor, this->rectangle);

  if (this->rectangle.w == 0 && this->rectangle.h == 0) {
    SDL_Rect textRectangle = this->text->getRectangle();
    this->rectangle.w      = textRectangle.w + 10;
    this->rectangle.h      = textRectangle.h + 10;
  }

  this->text->centerHorizontal(this->rectangle);
  this->text->centerVertical(this->rectangle);
}

/**
 * Set basic properties and add/position the text within the button. Button constructor
 * overload to assign a click return value so that the button will return a number other
 * than -1 when clicked.
 *
 * @param displayGlobal Global display variables
 * @param rectangle Rectangle to render the button with
 * @param textContent The text to print in the middle of the button
 * @param clickRet Value corrosponding to the button being clicked
 */
Button::Button(struct DisplayGlobal displayGlobal,
               const SDL_Rect& rectangle,
               const std::string& textContent,
               const int& clickRet)
    : clickReturn(clickRet) {
  this->displayGlobal = displayGlobal;
  this->rectangle     = rectangle;

  this->backgroundColor = {255, 0, 0, 255}; // Red
  this->hoveredColor    = {0, 255, 0, 255}; // Green
  this->defaultColor    = {255, 0, 0, 255}; // Red

  SDL_Color textColor = {255, 255, 0, 255}; // Yellow
  this->text =
      std::make_unique<Text>(this->displayGlobal, "../display/fonts/16020_FUTURAM.ttf",
                             textContent.c_str(), 24, textColor, this->rectangle);

  if (this->rectangle.w == 0 && this->rectangle.h == 0) {
    SDL_Rect textRectangle = this->text->getRectangle();
    this->rectangle.w      = textRectangle.w + 10;
    this->rectangle.h      = textRectangle.h + 10;
  }

  this->text->centerHorizontal(this->rectangle);
  this->text->centerVertical(this->rectangle);
}

/**
 * Check if the mouse is over the button.
 *
 * @param mouseXPosition X position of the mouse
 * @param mouseYPosition Y position of the mouse
 * @return Whether or not the mouse is over the button
 */
bool Button::checkHovered(const int& mouseXPosition, const int& mouseYPosition) {
  // Outside left edge of button
  if (mouseXPosition < this->rectangle.x) {
    return false;
  }

  // Outside right edge of button
  if (mouseXPosition > this->rectangle.x + this->rectangle.w) {
    return false;
  }

  // Outside top edge of button
  if (mouseYPosition < this->rectangle.y) {
    return false;
  }

  // Outside bottom edge of button
  if (mouseYPosition > this->rectangle.y + this->rectangle.h) {
    return false;
  }

  return true;
}

int Button::getClickReturn() const { return this->clickReturn; }

/**
 * Button operation to be performed periodically. Changes color if hovered and ensures
 * text is centered within button.
 *
 * @param None
 * @return None
 */
void Button::update() {
  // Change color if hovered
  int mouseXPosition, mouseYPosition;
  SDL_GetMouseState(&mouseXPosition, &mouseYPosition);
  if (checkHovered(mouseXPosition, mouseYPosition)) {
    this->backgroundColor = this->hoveredColor;
  }
  else {
    this->backgroundColor = this->defaultColor;
  }

  // Ensure text is centered
  if (this->text->checkCenterHorizontal(this->rectangle) == false) {
    this->text->centerHorizontal(this->rectangle);
  }
  if (this->text->checkCenterVertical(this->rectangle) == false) {
    this->text->centerVertical(this->rectangle);
  }
}

void Button::render() const {
  // Set draw color and fill the button
  SDL_SetRenderDrawColor(this->displayGlobal.renderer, backgroundColor.r,
                         backgroundColor.g, backgroundColor.b, backgroundColor.a);
  SDL_RenderFillRect(this->displayGlobal.renderer, &this->rectangle);
  this->text->render();
  if (this->hasBorder) {
    renderBorder();
  }
}
