#include <SDL2/SDL_image.h>

#include "bird.h"

Bird::Bird(const struct DisplayGlobal& displayGlobal,
           const std::string& logFile,
           const SDL_Rect boundaryRectangle)
    : Element(displayGlobal, logFile, boundaryRectangle) {
  this->debugName = "bird";

  SDL_Surface* birdSurface = IMG_Load("../display/sprites/bird.png");
  if (birdSurface == NULL) {
    std::cerr << "Error creating surface";
    exit(1);
  }

  this->texture = SDL_CreateTextureFromSurface(this->displayGlobal.renderer, birdSurface);
  if (this->texture == 0) {
    std::cerr << "error creating texture from surface";
    exit(1);
  }
  SDL_FreeSurface(birdSurface);

  this->fixed           = false;
  this->gravityAffected = true;
  this->canCollide      = true;
  this->collisionFixed  = false;
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
