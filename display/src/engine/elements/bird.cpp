#include <SDL2/SDL_image.h>
#include <glog/logging.h>

#include "bird.h"

Bird::Bird(struct DisplayGlobal displayGlobal, const SDL_Rect& boundaryRectangle) {
  this->displayGlobal     = displayGlobal;
  this->boundaryRectangle = boundaryRectangle;
  this->debugName         = "bird";

  SDL_Surface* birdSurface = IMG_Load("../display/sprites/bird.png");
  if (birdSurface == NULL) {
    LOG(FATAL) << "error creating surface";
  }

  this->texture = SDL_CreateTextureFromSurface(this->displayGlobal.renderer, birdSurface);
  if (this->texture == 0) {
    LOG(FATAL) << "error creating texture from surface";
  }
  SDL_FreeSurface(birdSurface);

  this->fixed          = false;
  this->canCollide     = true;
  this->collisionFixed = false;
}

/**
 * If the mouse button is pressed down, make the bird jump upwards.
 *
 * @param event SDL event that occured
 * @return None
 */
void Bird::handleEvent(const SDL_Event& event) {
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    this->velocity.y = -5;
  }
}

void Bird::render() const {
  SDL_RenderCopy(this->displayGlobal.renderer, this->texture, NULL,
                 &this->boundaryRectangle);
}
