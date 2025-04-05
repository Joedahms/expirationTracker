#ifndef BIRD_H
#define BIRD_H

#include "element.h"

class Bird : public Element {
public:
  Bird(struct DisplayGlobal displayGlobal, const SDL_Rect& boundaryRectangle);
  void handleEvent(const SDL_Event& event) override;
  void render() const override;

private:
  SDL_Texture* texture;
};

#endif
