#ifndef VISION_PIPE_H
#define VISION_PIPE_H

#include "../../food_item.h"
#include "../../pipes.h"
#include <string>
#include <vector>

void visionEntry(struct Pipes);
void processImages(struct Pipes, struct FoodItem&);
void closeUnusedPipes(struct Pipes);
#endif
