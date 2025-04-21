#ifndef FLAPPY_FOOD_H
#define FLAPPY_FOOD_H

#include "../../display_global.h"
#include "../composite_element.h"
#include "../text.h"
#include "bird.h"
#include "obstacle.h"
#include "obstacle_pair.h"

class FlappyFood : public CompositeElement {
public:
  FlappyFood(const struct DisplayGlobal& displayGlobal,
             const std::string& logFile,
             const SDL_Rect boundaryRectangle);

private:
  Bird* birdPtr = nullptr;
  std::vector<std::shared_ptr<ObstaclePair>> obstaclePairs;
  int score = 0;

  std::shared_ptr<Text> scoreText;

  void initializeObstacles();
  std::vector<SDL_Rect> getBoundaryRectangles();

  void handleBirdCollision();

  void updateSelf() override;
  void handleEventSelf(const SDL_Event& event) override;
};

#endif
