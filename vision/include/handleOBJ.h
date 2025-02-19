#ifndef HANDLE_OBJ_H
#define HANDLE_OBJ_H
#include "../../food_item.h"
#include <filesystem>
#include <iostream>

std::pair<int, float> runEfficientNet(const std::filesystem::path&);
bool handleObjectClassification(const std::filesystem::path&, struct FoodItem&);
#endif