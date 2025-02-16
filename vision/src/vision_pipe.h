#ifndef VISION_PIPE_H
#define VISION_PIPE_H

#include "../../pipes.h"
#include <string>
#include <vector>

void visionEntry(struct Pipes pipes);
void processFoodItems(struct Pipes, std::string, struct FoodItem);
bool analyzeImages(const std::string&);

#endif
