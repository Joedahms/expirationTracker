#include <SDL2/SDL.h>
#include <iostream>
#include <memory>

#include "button.h"
#include "display_global.h"

/*
 * Name: Button
 * Purpose: Set the properties of the button
 * Input:
 * - Global display variables
 * - Rectangle to render the button with
 * - The text to print in the middle of the button
 * Output: None
 */
Button::Button(struct DisplayGlobal displayGlobal,
               SDL_Rect rectangle,
               const std::string& text) {
  this->displayGlobal = displayGlobal;

  this->backgroundRectangle = rectangle;
  this->backgroundColor     = {255, 0, 0, 255}; // Red
  this->hoveredColor        = {0, 255, 0, 255}; // Green
  this->defaultColor        = {255, 0, 0, 255}; // Red

  SDL_Color textColor = {255, 255, 0, 255}; // Yellow
  this->text = std::make_unique<Text>(this->displayGlobal, "../16020_FUTURAM.ttf",
                                      text.c_str(), 24, textColor, rectangle);

  // Center the text within the button
  this->text->centerHorizontal(&this->backgroundRectangle);
  this->text->centerVertical(&this->backgroundRectangle);
}

/*
 * Name: checkHovered
 * Purpose: Check if the mouse is over the button
 * Input:
 * - X position of the mouse
 * - Y position of the mouse
 * Output: Whether or not the mouse is over the button
 */
bool Button::checkHovered(int mouseXPosition, int mouseYPosition) {
  if (mouseXPosition < this->backgroundRectangle.x) { // Outside left edge of button
    return false;
  }
  if (mouseXPosition > this->backgroundRectangle.x +
                           this->backgroundRectangle.w) { // Outside right edge of button
    return false;
  }
  if (mouseYPosition < this->backgroundRectangle.y) { // Outside top edge of button
    return false;
  }
  if (mouseYPosition > this->backgroundRectangle.y +
                           this->backgroundRectangle.h) { // Outside bottom edge of button
    return false;
  }
  return true;
}

/*
 * Name: render
 * Purpose: Render the button
 * Input:
 * - None
 * Output: None
 */
void Button::render() {
  // Change color if hovered
  int mouseXPosition, mouseYPosition;
  SDL_GetMouseState(&mouseXPosition, &mouseYPosition); // Get the position of the mouse
  if (checkHovered(mouseXPosition, mouseYPosition)) {  // Mouse is hovered over the button
    this->backgroundColor = this->hoveredColor;        // Change to hovered color
  }
  else {                                        // Mouse is not hovered over the button
    this->backgroundColor = this->defaultColor; // Change to default color
  }

  // Set draw color and fill the button
  SDL_SetRenderDrawColor(this->displayGlobal.renderer, backgroundColor.r,
                         backgroundColor.g, backgroundColor.b, backgroundColor.a);
  SDL_RenderFillRect(this->displayGlobal.renderer, &this->backgroundRectangle);

  this->text->render();
}
