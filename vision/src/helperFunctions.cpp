#include "../include/helperFunctions.h"

#include "../../pipes.h"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <memory>
#include <vector>

/**
 * Determine if a path is valid directory
 *
 * Input:
 * @param directory path to directory
 * Output: bool on whether is a valid directory
 */
bool isValidDirectory(const std::filesystem::path& directory) {
  if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
    return false;
  }
  return true;
}

/**
 * Determine if a given directory has files
 *
 * Input:
 * @param dirPath path to directory to examine
 * Output: bool on whether or not directory has files
 */
bool hasFiles(const std::filesystem::path& dirPath) {
  return std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath) &&
         !std::filesystem::is_empty(dirPath);
}

/**
 * Wrapper method to close pipes for vision
 *
 * Input:
 * @param pipes pipe struct to close from
 * Output: none
 */
void closeUnusedPipes(struct Pipes& pipes) {
  // Close write end of read pipes
  close(pipes.displayToVision[WRITE]);
  close(pipes.hardwareToVision[WRITE]);

  // Close read end of write pipes
  close(pipes.visionToDisplay[READ]);
  close(pipes.visionToHardware[READ]);
}

/**
 * perform levenshtein distancing
 *
 * Input:
 * @param s1 string 1
 * @param s2 string 2
 * Output: none
 * @return the difference between the strings
 */
int levenshteinDistance(const std::string& s1, const std::string& s2) {
  const size_t len1 = s1.size(), len2 = s2.size();
  std::vector<std::vector<int>> d(len1 + 1, std::vector<int>(len2 + 1));

  for (size_t i = 0; i <= len1; ++i)
    d[i][0] = i;
  for (size_t j = 0; j <= len2; ++j)
    d[0][j] = j;

  for (size_t i = 1; i <= len1; ++i) {
    for (size_t j = 1; j <= len2; ++j) {
      int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
      d[i][j]  = std::min({d[i - 1][j] + 1,          // Deletion
                           d[i][j - 1] + 1,          // Insertion
                           d[i - 1][j - 1] + cost}); // Substitution
    }
  }
  return d[len1][len2];
}

/**
 * convert string to all lowercase
 *
 * Input:
 * @param str string to convert
 * @return lowercase string
 */
std::string toLowerCase(std::string str) {
  std::transform(str.begin(), str.end(), str.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return str;
}

/**
 * split long strings into individual words
 *
 * Input:
 * @param text text to split apart
 * Output:
 * @return vector of strings
 */
std::vector<std::string> splitWords(const std::string& text) {
  std::vector<std::string> words;
  std::istringstream iss(text);
  std::string word;
  while (iss >> word) {
    words.push_back(word);
  }
  return words;
}

/**
 * Remove special characters
 *
 * Input:
 * @param text text clean
 * Output:
 * @return cleaned text
 */
std::string cleanText(const std::string& text) {
  std::string result;
  for (char c : text) {
    if (std::isalnum(c) || std::isspace(c)) {
      result += c;
    }
  }
  return result;
}