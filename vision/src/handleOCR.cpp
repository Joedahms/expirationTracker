#include "handleOCR.h"

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

std::string runEfficientNet(const std::string& imagePath) {
  std::string command = "TF_CPP_MIN_LOG_LEVEL=3 $HOME/easyocr-venv/bin/python3 "
                        "../vision/Models/efficientNet.py " +
                        imagePath + " 2>/dev/null";

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
