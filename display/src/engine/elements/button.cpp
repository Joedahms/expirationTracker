#include <SDL2/SDL.h>
#include <iostream>
#include <memory>

#include "../display_global.h"
#include "button.h"
#include "element.h"

/**
 *
 *
 * @param displayGlobal Global display variables
 * @param rectangle Rectangle to render the button with
 * @param textContent The text to print in the middle of the button
 * @param clickRet Value corrosponding to the button being clicked
 */
Button::Button(struct DisplayGlobal displayGlobal,
               const SDL_Rect& rectangle,
               const std::string& textContent,
               std::function<void()> callback)
    : onClick(callback) {
  this->displayGlobal = displayGlobal;
  this->rectangle     = rectangle;

  // Colors
  this->backgroundColor = {255, 0, 0, 255}; // Red
  this->hoveredColor    = {0, 255, 0, 255}; // Green
  this->defaultColor    = {255, 0, 0, 255}; // Red

  // Button Text
  SDL_Color textColor = {255, 255, 0, 255}; // Yellow
  std::unique_ptr<Text> text =
      std::make_unique<Text>(this->displayGlobal, "../display/fonts/16020_FUTURAM.ttf",
                             textContent.c_str(), 24, textColor, this->rectangle);

  // Size based on text
  if (this->rectangle.w == 0 && this->rectangle.h == 0) {
    SDL_Rect textRectangle = text->getRectangle();
    this->rectangle.w      = textRectangle.w + 10;
    this->rectangle.h      = textRectangle.h + 10;
  }

  // Center text
  text->centerHorizontal(this->rectangle);
  text->centerVertical(this->rectangle);

  addElement(std::move(text));
}

void Button::update() {
  updateSelf();
  for (const auto& element : this->children) {
    element->update();
    if (element->checkCenterHorizontal(this->rectangle) == false) {
      element->centerHorizontal(this->rectangle);
    }
    if (element->checkCenterVertical(this->rectangle) == false) {
      element->centerVertical(this->rectangle);
    }
  }
}

/**
 * Button operation to be performed periodically. Changes color if hovered and ensures
 * text is centered within button.
 *
 * @param None
 * @return None
 */
void Button::updateSelf() {
  // Change color if hovered
  int mouseXPosition, mouseYPosition;
  SDL_GetMouseState(&mouseXPosition, &mouseYPosition);
  if (checkHovered(mouseXPosition, mouseYPosition)) {
    this->backgroundColor = this->hoveredColor;
  }
  else {
    this->backgroundColor = this->defaultColor;
  }
}

void Button::renderSelf() const {
  // Set draw color and fill the button
  SDL_SetRenderDrawColor(this->displayGlobal.renderer, backgroundColor.r,
                         backgroundColor.g, backgroundColor.b, backgroundColor.a);
  SDL_RenderFillRect(this->displayGlobal.renderer, &this->rectangle);
}

void Button::handleEventSelf(const SDL_Event& event) {
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    if (checkHovered(event.motion.x, event.motion.y) == true) {
      onClick();
    }
  }
}

/**
 * Check if the mouse is over the button. Could be improved by using SDL_Point.
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
