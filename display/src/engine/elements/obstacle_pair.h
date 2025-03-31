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

private:
  void updateSelf() override;
  void handleEventSelf(const SDL_Event& event);

  const int respawnOffset;

  const int windowWidth;

  const int obstacleWidth         = 40;
  const int obstacleMinHeight     = 40;
  const int obstacleMaxHeight     = 100;
  const int obstaclePairHeight    = 200;
  const int verticalObstacleGap   = 50;
  const int horizontalObstacleGap = 70;
};

#endif
