#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

#include <filesystem>
#include <string>
#include <vector>

bool isValidDirectory(const std::filesystem::path&);
bool hasFiles(const std::filesystem::path&);
void closeUnusedPipes(struct Pipes&);
std::string toLowerCase(std::string);
int levenshteinDistance(const std::string&, const std::string&);
std::vector<std::string> splitWords(const std::string&);
std::string cleanText(const std::string&);
#endif