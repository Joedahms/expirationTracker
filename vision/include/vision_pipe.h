#ifndef VISION_PIPE_H
#define VISION_PIPE_H

#include "../../food_item.h"
#include "../../pipes.h"
#include <string>
#include <vector>

void visionEntry(struct Pipes pipes);
void processImages(struct Pipes, std::string, struct FoodItem);

#endif
