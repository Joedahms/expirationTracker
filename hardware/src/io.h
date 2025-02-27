#ifndef IO_H
#define IO_H

#include "../../pipes.h"
#include <string>

bool receivedStartSignal(int pipeToRead);
<<<<<<< HEAD
void sendDataToVision(int pipeToWrite, float weight);
=======
void sendDataToVision(float weight);
>>>>>>> 76ce66bc0a4ce61b4b5f6e5b890c68f0c271a536
void sendImagesWithinDirectory(int pipeToWrite, const std::string& directory_path);

#endif