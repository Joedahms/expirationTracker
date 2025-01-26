#ifndef CATEGORIZE_OBJECTS_H
#define CATEGORIZE_OBJECTS_H

#include <string>

std::string analyzeImage(const std::string imagePath,
                         const std::string configPath,
                         const std::string weightsPath,
                         const std::string namesPath);

void suppressOutput();
void restoreOutput();

#endif
