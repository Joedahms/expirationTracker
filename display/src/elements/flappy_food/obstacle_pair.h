#ifndef OBSTACLE_PAIR_H
#define OBSTACLE_PAIR_H

#include "../composite_element.h"

class ObstaclePair : public CompositeElement {
public:
  bool scored = false;

  ObstaclePair(const struct DisplayGlobal& displayGlobal,
               const std::string& logFile,
               const SDL_Rect boundaryRectangle,
               const int windowWidth,
               const int respawnOffset,
               const int minHeight,
               const int verticalGap);

  SDL_Rect getTopObstacleRect();
  SDL_Rect getBottomObstacleRect();
  void reset();

private:
  const int windowWidth;
  const int respawnOffset;
  const int minHeight;
  const int verticalGap;
  const int startPosition;

  void randomizeGapPosition();
  void updateSelf() override;
  void handleEventSelf(const SDL_Event& event);
};

#endif
