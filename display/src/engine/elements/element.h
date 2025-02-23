#ifndef ELEMENT_H
#define ELEMENT_H

#include <SDL2/SDL.h>
#include <memory>

#include "../display_global.h"

class Element {
public:
  virtual ~Element() = default;
  virtual void addElement(std::unique_ptr<Element> element) {}
  virtual void update();
  virtual void render() const                      = 0;
  virtual void handleEvent(const SDL_Event& event) = 0;

  virtual void setContent(const std::string& content) {}
  virtual std::string getContent() const { return "no content"; }

  void setPositionRelativeToParent(const SDL_Point& relativePosition);
  SDL_Point getPositionRelativeToParent() { return this->positionRelativeToParent; }
  void setParent(Element* parent);

  SDL_Rect getBoundaryRectangle() { return this->boundaryRectangle; }
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
