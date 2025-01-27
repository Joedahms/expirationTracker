#ifndef CATEGORIZE_OBJECTS_H
#define CATEGORIZE_OBJECTS_H

#include <string>
#include <vector>

std::string analyzeImage(const std::string imagePath,
                         const std::string configPath,
                         const std::string weightsPath,
                         const std::string namesPath);

void suppressOutput();
void restoreOutput();
std::vector<std::string> loadClassLabels(const std::string);

#endif
