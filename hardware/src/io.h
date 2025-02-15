#ifndef IO_H
#define IO_H

#include "../../pipes.h"
#include <string>

bool receivedStartSignal(int pipeToRead);
void sendDataToVision(const std::string IMAGE_DIR, float weight);
void sendImagesWithinDirectory(int pipeToWrite, const std::string& directory_path);

#endif