#include "../include/helperFunctions.h"

#include "../../pipes.h"
#include <filesystem>
#include <memory>

bool isValidDirectory(const std::filesystem::path& directory) {
  if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
    return false;
  }
  return true;
}

void closeUnusedPipes(struct Pipes& pipes) {
  // Close write end of read pipes
  close(pipes.displayToVision[WRITE]);
  close(pipes.hardwareToVision[WRITE]);

  // Close read end of write pipes
  close(pipes.visionToDisplay[READ]);
  close(pipes.visionToHardware[READ]);
}