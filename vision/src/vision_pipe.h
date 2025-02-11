#ifndef VISION_PIPE_H
#define VISION_PIPE_H

#include "../../pipes.h"
#include <string>
#include <vector>

void visionEntry(struct Pipes pipes);
void redoThisFunctionPlz(struct Pipes pipes);
void receiveImages(int, const std::string&);
std::vector<std::string> analyzeImages(const std::string&);

#endif
