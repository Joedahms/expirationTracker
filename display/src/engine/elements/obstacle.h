#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "element.h"

class Obstacle : public Element {
public:
  Obstacle(struct DisplayGlobal displayGlobal, const SDL_Rect& boundaryRectangle);
  void handleEvent(const SDL_Event& event) override;
  void render() const override;
};

#endif
