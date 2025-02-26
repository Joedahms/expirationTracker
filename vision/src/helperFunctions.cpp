#include "../include/helperFunctions.h"

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