#include "../include/helperFunctions.h"

#include "../../pipes.h"
#include <filesystem>
#include <memory>

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