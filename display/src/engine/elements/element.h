#ifndef ELEMENT_H
#define ELEMENT_H

#include <SDL2/SDL.h>
#include <memory>

#include "../display_global.h"

/**
 * Defines the interface for an SDL element. This is any basic shape or texture to be
 * rendered to the screen.
 */
class Element {
public:
  virtual ~Element() = default;
  virtual void addElement(std::unique_ptr<Element> element) {}
  virtual void update();
  virtual void render() const                      = 0;
  virtual void handleEvent(const SDL_Event& event) = 0;

  virtual std::string getContent() const { return "no content"; }
  virtual void setContent(const std::string& content) {}

  void setParent(Element* parent);

  SDL_Point getPositionRelativeToParent();
  void setPositionRelativeToParent(const SDL_Point& relativePosition);

  SDL_Rect getBoundaryRectangle();
  void setboundaryRectangle(SDL_Rect boundaryRectangle);

  void setCentered();

  void setCenteredVertical();
  bool checkCenterVertical();
  void centerVertical();

  void setCenteredHorizontal();
  bool checkCenterHorizontal();
  void centerHorizontal();

  void addBorder(const int& borderThickness);
  void renderBorder() const;

protected:
  struct DisplayGlobal displayGlobal;
  SDL_Rect boundaryRectangle         = {0, 0, 0, 0};
  SDL_Point positionRelativeToParent = {0, 0};
  Element* parent                    = nullptr;
  bool hasBorder                     = false;
  bool centerWithinParent            = false;
  bool centerVerticalWithinParent    = false;
  bool centerHorizontalWithinParent  = false;
  int borderThickness                = 1;
};

#endif
