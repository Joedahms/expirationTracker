#include "handleOCR.h"

/**
 * Entry into the vision code. Only called from main after vision child process is
 * forked.
 *
 * Input:
 * @param pipes Pipes for vision to communicate with the other processes
 * Output: None
 */

std::string runOCR(const std::string& imagePath) {
  std::string command =
      "$HOME/easyocr-venv/bin/python3 ../vision/Models/easyOCR.py " + imagePath;
  std::string result;
  char buffer[256];

  FILE* pipe = popen(command.c_str(), "r");
  if (!pipe)
    return "ERROR";

  while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
    result += buffer;
  }
  pclose(pipe);
  return result;
}

/**
 * Method to handle calling python script to run object classification.
 *
 * Input:
 * @param imagePath path to the image you wish to classify
 * Output: Returns the class index of the identified class (currently from imageNet).
 */

int runEfficientNet(const std::string& imagePath) {
  std::string command = "TF_CPP_MIN_LOG_LEVEL=3 $HOME/easyocr-venv/bin/python3 "
                        "../vision/Models/efficientNet.py " +
                        imagePath + " 2>/dev/null";

  std::string result;
  char buffer[256];

  FILE* pipe = popen(command.c_str(), "r");
  if (!pipe)
    return -1;

  while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
    result += buffer;
  }
  pclose(pipe);
  return std::stoi(result);
}
