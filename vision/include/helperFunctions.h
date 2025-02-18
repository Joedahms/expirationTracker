#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

#include <filesystem>

bool isValidDirectory(const std::filesystem::path&);

void closeUnusedPipes(struct Pipes&);
#endif