#ifndef CATEGORIZE_OBJECTS_H
#define CATEGORIZE_OBJECTS_H

#include <string>

std::string analyzeImage(const char* imagePath,
                         const char* configPath,
                         const char* weightsPath,
                         const char* namesPath);

#endif
