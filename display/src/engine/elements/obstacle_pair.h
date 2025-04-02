#ifndef OBSTACLE_PAIR_H
#define OBSTACLE_PAIR_H

#include "composite_element.h"

class ObstaclePair : public CompositeElement {
public:
  ObstaclePair(struct DisplayGlobal displayGlobal,
               const SDL_Rect& boundaryRectangle,
               int windowWidth,
               int respawnOffset,
               std::string logFile);

  bool scored = false;

private:
  const int windowWidth;
  const int respawnOffset;

  const int obstacleMinHeight   = 40;
  const int obstacleMaxHeight   = 100;
  const int obstaclePairHeight  = 200;
  const int verticalObstacleGap = 50;

  void randomizeGapPosition();
  void updateSelf() override;
  void handleEventSelf(const SDL_Event& event);
};

#endif
