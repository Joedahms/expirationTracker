#include <random>

#include "obstacle.h"
#include "obstacle_pair.h"

ObstaclePair::ObstaclePair(const struct DisplayGlobal& displayGlobal,
                           const SDL_Rect& boundaryRectangle,
                           const int windowWidth,
                           const int respawnOffset,
                           const int minHeight,
                           const int verticalGap,
                           const std::string& logFile)
    : windowWidth(windowWidth), respawnOffset(respawnOffset), minHeight(minHeight),
      verticalGap(verticalGap) {
  this->displayGlobal = displayGlobal;
  setupPosition(boundaryRectangle);
  this->logger = std::make_unique<Logger>(logFile);
  this->logger->log("Constructing obstacle pair");
  this->velocity.x   = -3;
  this->fixed        = false;
  this->screenBoundX = false;

  std::unique_ptr<Obstacle> topObstacle = std::make_unique<Obstacle>(
      this->displayGlobal, SDL_Rect{0, 0, this->boundaryRectangle.w, 0});
  addElement(std::move(topObstacle));

  std::unique_ptr<Obstacle> bottomObstacle = std::make_unique<Obstacle>(
      this->displayGlobal, SDL_Rect{0, 0, this->boundaryRectangle.w, 0});
  addElement(std::move(bottomObstacle));

  randomizeGapPosition();

  this->logger->log("Obstacle pair constructed");
}

SDL_Rect ObstaclePair::getTopObstacleRect() {
  SDL_Rect rect = this->children[0]->getBoundaryRectangle();
  return rect;
}

SDL_Rect ObstaclePair::getBottomObstacleRect() {
  SDL_Rect rect = this->children[1]->getBoundaryRectangle();
  return rect;
}

/**
 * Randomize how long the top and bottom pipes are.
 *
 * @param None
 * @return None
 */
void ObstaclePair::randomizeGapPosition() {
  SDL_Rect topRect = this->children[0]->getBoundaryRectangle();

  std::random_device r;
  std::default_random_engine e1(r());
  int maxHeight = this->boundaryRectangle.h - this->verticalGap - this->minHeight;
  std::uniform_int_distribution<int> uniform_dist(minHeight, maxHeight);
  int topHeight = uniform_dist(e1);
  topRect.h     = topHeight;
  this->children[0]->setBoundaryRectangle(topRect);

  int relativeYPosition = topHeight + this->verticalGap;
  int bottomHeight      = this->boundaryRectangle.h - topHeight - this->verticalGap;

  SDL_Rect bottomRect                = this->children[1]->getBoundaryRectangle();
  SDL_Point positionRelativeToParent = this->children[1]->getPositionRelativeToParent();
  positionRelativeToParent.y         = relativeYPosition;
  bottomRect.h                       = bottomHeight;
  this->children[1]->setBoundaryRectangle(bottomRect);
  this->children[1]->setPositionRelativeToParent(positionRelativeToParent);
}

/**
 * Perform child update. Check if off screen and loop back around if so.
 *
 * @param None
 * @return None
 */
void ObstaclePair::updateSelf() {
  if (parent) {
    hasParentUpdate();
  }

  if (this->boundaryRectangle.x <= -(this->boundaryRectangle.w)) {
    this->positionRelativeToParent.x = this->windowWidth + this->respawnOffset;
    this->scored                     = false;
    randomizeGapPosition();
    updatePosition();
  }
}

void ObstaclePair::handleEventSelf(const SDL_Event& event) {}
