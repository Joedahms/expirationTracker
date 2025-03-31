#include <random>

#include "obstacle.h"
#include "obstacle_pair.h"

ObstaclePair::ObstaclePair(DisplayGlobal displayGlobal,
                           const SDL_Rect& boundaryRectangle,
                           int windowWidth,
                           int respawnOffset,
                           std::string logFile)
    : windowWidth(windowWidth), respawnOffset(respawnOffset) {
  this->displayGlobal = displayGlobal;
  this->logger        = std::make_unique<Logger>(logFile);
  this->logger->log("Constructing obstacle pair");
  setupPosition(boundaryRectangle);
  this->velocity.x   = -3;
  this->fixed        = false;
  this->screenBoundX = false;

  /*
  // Random top obstacle height
  std::random_device r;
  std::default_random_engine e1(r());
  std::uniform_int_distribution<int> uniform_dist(this->obstacleMinHeight,
                                                  this->obstacleMaxHeight);
  int obstacleHeight = uniform_dist(e1);

  SDL_Rect topObstacleRect = {0, 0, this->boundaryRectangle.w, obstacleHeight};
  std::unique_ptr<Obstacle> topObstacle =
      std::make_unique<Obstacle>(this->displayGlobal, topObstacleRect);
  addElement(std::move(topObstacle));

  int yPosition  = this->verticalObstacleGap + obstacleHeight;
  obstacleHeight = obstaclePairHeight - obstacleHeight - verticalObstacleGap;

  SDL_Rect bottomObstacleRect = {0, yPosition, this->boundaryRectangle.w, obstacleHeight};
  std::unique_ptr<Obstacle> bottomObstacle =
      std::make_unique<Obstacle>(this->displayGlobal, bottomObstacleRect);
  addElement(std::move(bottomObstacle));
  */

  std::unique_ptr<Obstacle> topObstacle = std::make_unique<Obstacle>(
      this->displayGlobal, SDL_Rect{0, 0, this->boundaryRectangle.w, 0});
  addElement(std::move(topObstacle));

  std::unique_ptr<Obstacle> bottomObstacle = std::make_unique<Obstacle>(
      this->displayGlobal, SDL_Rect{0, 0, this->boundaryRectangle.w, 0});
  addElement(std::move(bottomObstacle));

  randomizeGapPosition();

  this->logger->log("Obstacle pair constructed");
}

void ObstaclePair::randomizeGapPosition() {
  SDL_Rect boundaryRectangle = this->children[0]->getBoundaryRectangle();

  std::random_device r;
  std::default_random_engine e1(r());
  std::uniform_int_distribution<int> uniform_dist(this->obstacleMinHeight,
                                                  this->obstacleMaxHeight);
  int obstacleHeight  = uniform_dist(e1);
  boundaryRectangle.h = obstacleHeight;
  this->children[0]->setBoundaryRectangle(boundaryRectangle);

  int yPosition  = this->verticalObstacleGap + obstacleHeight;
  obstacleHeight = obstaclePairHeight - obstacleHeight - verticalObstacleGap;

  boundaryRectangle                  = this->children[1]->getBoundaryRectangle();
  SDL_Point positionRelativeToParent = this->children[1]->getPositionRelativeToParent();
  positionRelativeToParent.y         = yPosition;
  boundaryRectangle.h                = obstacleHeight;
  this->children[1]->setBoundaryRectangle(boundaryRectangle);
  this->children[1]->setPositionRelativeToParent(positionRelativeToParent);
}

void ObstaclePair::updateSelf() {
  if (parent) {
    hasParentUpdate();
  }

  if (this->boundaryRectangle.x <= -(this->boundaryRectangle.w)) {
    this->positionRelativeToParent.x = this->windowWidth + this->respawnOffset;
    randomizeGapPosition();
    updatePosition();
  }
}

void ObstaclePair::handleEventSelf(const SDL_Event& event) {}
