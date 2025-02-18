#ifndef HANDLEOCR_H
#define HANDLEOCR_H
#include "../../food_item.h"
#include <filesystem>
#include <vector>

std::vector<std::string> runOCR(const std::filesystem::path&);
void handleTextExtraction(const std::filesystem::path&, struct FoodItem&, bool&, bool&);
#endif