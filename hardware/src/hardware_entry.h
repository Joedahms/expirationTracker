#ifndef HARDWARE_ENTRY_H
#define HARDWARE_ENTRY_H

#include "../../pipes.h"
#include <string>

void hardwareEntry(struct HardwarePipes);
void sendImagesWithinDirectory(int, const std::string&);

#endif