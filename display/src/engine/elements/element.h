#ifndef ELEMENT_H
#define ELEMENT_H

#include <SDL2/SDL.h>
#include <memory>

#include "../display_global.h"

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
    this->boundaryRectangle.y = (centerWithin->h / 2 - this->boundaryRectangle.h / 2);
  }

  /**
   * Center an element in the horizontal direction. Centers within
   * another object with a w(width) member.
   *
   * @param centerWithin The object to center within
   * @return None
   */
  template <typename T> void centerHorizontal(T centerWithin) {
    this->boundaryRectangle.x = (centerWithin->w / 2 - this->boundaryRectangle.w / 2);
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
    if (this->boundaryRectangle.y ==
        (centerWithin->h / 2 - this->boundaryRectangle.h / 2)) {
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
    if (this->boundaryRectangle.x ==
        (centerWithin->w / 2 - this->boundaryRectangle.w / 2)) {
      centered = true;
    }
    return centered;
  }

  virtual void addElement(std::unique_ptr<Element> element) {}
  virtual void update();
  virtual void render() const                      = 0;
  virtual void handleEvent(const SDL_Event& event) = 0;

  void setPositionRelativeToParent(const SDL_Point& relativePosition);
  void setParent(Element* parent);

  SDL_Rect getBoundaryRectangle() { return this->boundaryRectangle; }
  void setboundaryRectangle(SDL_Rect boundaryRectangle);
  void addBorder(const int& borderThickness);
  void renderBorder() const;

protected:
  struct DisplayGlobal displayGlobal;
  SDL_Rect boundaryRectangle         = {0, 0, 0, 0};
  SDL_Point positionRelativeToParent = {0, 0};
  Element* parent                    = nullptr;
  bool hasBorder                     = false;
  int borderThickness                = 1;
};

template <> void Element::centerVertical<SDL_Rect>(SDL_Rect centerWithin);
template <> void Element::centerHorizontal<SDL_Rect>(SDL_Rect centerWithin);

template <> bool Element::checkCenterVertical<SDL_Rect>(SDL_Rect centerWithin);
template <> bool Element::checkCenterHorizontal<SDL_Rect>(SDL_Rect centerWithin);

#endif
