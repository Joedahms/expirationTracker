#include "handleOCR.h"

std::string runOCR(const std::string& imagePath) {
  std::string command = "python3 ocr_script.py " + imagePath;
  std::string result;
  char buffer[128];

  FILE* pipe = popen(command.c_str(), "r");
  if (!pipe)
    return "ERROR";

  while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
    result += buffer;
  }
  pclose(pipe);
  return result;
}
