#ifndef HARDWARE_PIPE_H
#define HARDWARE_PIPE_H

#include "../../pipes.h"
#include <string>

void hardwareEntry(struct HardwarePipes);
void sendImagesWithinDirectory(int, const std::string&);

#endif