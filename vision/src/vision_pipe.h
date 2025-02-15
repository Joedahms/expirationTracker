#ifndef VISION_PIPE_H
#define VISION_PIPE_H

#include "../../pipes.h"
#include <string>
#include <vector>

void visionEntry(struct Pipes pipes);
void processFoodItems(struct Pipes, std::string);
void sendFoodItems(int pipe);
void receiveImages(int, const std::string&);
std::vector<std::string> analyzeImages(const std::string&);

#endif
