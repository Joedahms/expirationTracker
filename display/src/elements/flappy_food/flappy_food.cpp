#include <assert.h>

#include "flappy_food.h"

FlappyFood::FlappyFood(const struct DisplayGlobal& displayGlobal,
                       const std::string& logFile,
                       const SDL_Rect boundaryRectangle)
    : CompositeElement(displayGlobal, logFile, boundaryRectangle) {
  this->logger->log("Constructing flappy food");

  SDL_Surface* windowSurface = SDL_GetWindowSurface(this->displayGlobal.window);
  assert(windowSurface != NULL);

  this->logger->log("Constructing bird");
  SDL_Rect birdRect = {20, 0, 32, 32};
  birdRect.y        = windowSurface->h - birdRect.h;
  std::unique_ptr<Bird> bird =
      std::make_unique<Bird>(this->displayGlobal, this->logFile, birdRect);
  this->birdPtr = bird.get();
  addElement(std::move(bird));
  this->logger->log("Bird constructed");

  initializeObstacles();

  this->logger->log("Constructing score text");
  const std::string scoreTextContent = "0";
  const SDL_Color scoreTextColor     = {0, 255, 0, 255}; // Green
  const SDL_Rect scoreTextRect       = {0, 150, 0, 0};
  this->scoreText = std::make_shared<Text>(this->displayGlobal, this->logFile,
                                           scoreTextRect, DisplayGlobal::futuramFontPath,
                                           scoreTextContent, 24, scoreTextColor);
  this->scoreText->setCenteredHorizontal();
  addElement(scoreText);
  this->logger->log("Score text constructed");

  this->logger->log("Flappy food constructed");
}

void FlappyFood::initializeObstacles() {
  const int pairWidth         = 40;
  const int pairHeight        = 400;
  const int horizontalPairGap = 150;
  const int minHeight         = 20;
  const int verticalGap       = 150;

  SDL_Surface* windowSurface = SDL_GetWindowSurface(this->displayGlobal.window);
  assert(windowSurface != NULL);

  const int windowWidth  = windowSurface->w;
  const int windowHeight = windowSurface->h;
  int xPosition          = windowWidth;
  const int yPosition    = windowHeight - pairHeight;

  const int totalPairs    = (windowWidth / (pairWidth + horizontalPairGap)) + 1;
  const int respawnOffset = windowWidth % (pairWidth + horizontalPairGap);

  for (int i = 0; i < totalPairs; i++) {
    SDL_Rect boundaryRectangle = {xPosition, yPosition, pairWidth, pairHeight};

    std::shared_ptr<ObstaclePair> obstaclePair = std::make_shared<ObstaclePair>(
        this->displayGlobal, this->logFile, boundaryRectangle, windowWidth, respawnOffset,
        minHeight, verticalGap);

    this->obstaclePairs.push_back(obstaclePair);

    addElement(std::move(obstaclePair));
    xPosition += pairWidth + horizontalPairGap;
  }
}

std::vector<SDL_Rect> FlappyFood::getBoundaryRectangles() {
  std::vector<SDL_Rect> boundaryRectangles;
  addBoundaryRectangle(boundaryRectangles);

  return boundaryRectangles;
}

void FlappyFood::handleBirdCollision() {
  if (this->birdPtr->getHasCollided()) {
    this->birdPtr->setVelocity(Velocity{0, 0});
    for (const auto& obstaclePair : this->obstaclePairs) {
      obstaclePair->setVelocity(Velocity{0, 0});
    }
  }
}

void FlappyFood::updateSelf() {
  if (parent) {
    hasParentUpdate();
  }

  containChildren();

  std::vector<SDL_Rect> boundaryRectangles = getBoundaryRectangles();
  checkCollision(boundaryRectangles);
  handleBirdCollision();

  SDL_Surface* windowSurface = SDL_GetWindowSurface(this->displayGlobal.window);
  assert(windowSurface != NULL);

  SDL_Point birdRelative = this->birdPtr->getPositionRelativeToParent();
  if (birdRelative.y < windowSurface->h - 400) {
    this->birdPtr->setVelocity(Velocity{0, 0});
    birdRelative.y++;
    this->birdPtr->setPositionRelativeToParent(birdRelative);
  }
  for (const auto& obstaclePair : this->obstaclePairs) {
    if (obstaclePair->scored) {
      continue;
    }

    SDL_Rect pairRect = obstaclePair->getBoundaryRectangle();
    if (birdRelative.x >= pairRect.x + pairRect.w) {
      this->score++;
      this->scoreText->setContent(std::to_string(this->score));
      obstaclePair->scored = true;
    }
  }
}

void FlappyFood::handleEventSelf(const SDL_Event& event) {}
