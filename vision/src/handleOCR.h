#ifndef HANDLEOCR_H
#define HANDLEOCR_H
#include <string>

std::string runOCR(const std::string& imagePath);
std::string runEfficientNet(const std::string& imagePath);
#endif