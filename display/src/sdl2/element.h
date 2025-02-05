#ifndef ELEMENT_H
#define ELEMENT_H

#include <SDL2/SDL.h>

#include "display_global.h"

class Element {
public:
  /**
   * Center an element in the vertical direction. Centers within
   * another object with a h(height) member.
   *
   * @param centerWithin The object to center within
   * @return None
   */
  template <typename T> void centerVertical(T centerWithin) {
    this->rectangle.y = (centerWithin->h / 2 - this->rectangle.h / 2);
  }

  /**
   * Center an element in the horizontal direction. Centers within
   * another object with a w(width) member.
   *
   * @param centerWithin The object to center within
   * @return None
   */
  template <typename T> void centerHorizontal(T centerWithin) {
    this->rectangle.x = (centerWithin->w / 2 - this->rectangle.w / 2);
  }

  /**
   * Check if an element is centered vertically within another
   * object with a h(height) member.
   *
   * @param centerWithin The object to center within
   * @return True if centered vertically, false if not centered vertically
   */
  template <typename T> bool checkCenterVertical(T centerWithin) {
    bool centered = false;
    if (this->rectangle.y == (centerWithin->h / 2 - this->rectangle.h / 2)) {
      centered = true;
    }
    return centered;
  }

  /**
   * Check if an element is centered horizontally within another
   * object with a w(width) member.
   *
   * @param centerWithin The object to center within
   * @return True if centered horizontally, false if not centered horizontally
   */
  template <typename T> bool checkCenterHorizontal(T centerWithin) {
    bool centered = false;
    if (this->rectangle.x == (centerWithin->w / 2 - this->rectangle.w / 2)) {
      centered = true;
    }
    return centered;
  }

  virtual void render() const = 0;
  SDL_Rect getRectangle() { return this->rectangle; }
  void setRectangle(SDL_Rect rectangle);
  void addBorder(const int& borderThickness);
  //  bool hasBorder() { return this->border; }
  void renderBorder() const;

protected:
  struct DisplayGlobal displayGlobal;
  SDL_Rect rectangle;
  bool hasBorder      = false;
  int borderThickness = 1;
};

template <> void Element::centerVertical<SDL_Rect>(SDL_Rect centerWithin);
template <> void Element::centerHorizontal<SDL_Rect>(SDL_Rect centerWithin);

template <> bool Element::checkCenterVertical<SDL_Rect>(SDL_Rect centerWithin);
template <> bool Element::checkCenterHorizontal<SDL_Rect>(SDL_Rect centerWithin);

#endif
