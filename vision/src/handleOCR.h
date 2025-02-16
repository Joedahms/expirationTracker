#ifndef HANDLEOCR_H
#define HANDLEOCR_H
#include <string>

std::string runOCR(const std::string& imagePath);
std::pair<int, float> runEfficientNet(const std::string& imagePath);
#endif