#include "../include/handleOCR.h"
#include <glog/logging.h>
#include <iostream>
#include <nlohmann/json.hpp>

/**
 * Method to handle calling python script to run text extraction.
 *
 * Input:
 * @param imagePath path to the image you wish to extract text from
 * Output: Vector of strings extracted from the image.
 */
std::vector<std::string> runOCR(const std::string& imagePath) {
  std::vector<std::string> detectedText;
  std::string command =
      "$HOME/easyocr-venv/bin/python3 ../vision/Models/easyOCR.py " + imagePath;
  char buffer[256];
  std::string result;

  FILE* pipe = popen(command.c_str(), "r");
  if (!pipe) {
    return {"ERROR"};
  }

  while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
    result += buffer;
  }
  pclose(pipe);

  try {
    // Parse JSON output
    nlohmann::json jsonData = nlohmann::json::parse(result);

    if (jsonData.is_array()) {
      for (const auto& text : jsonData) {
        detectedText.push_back(text.get<std::string>());
      }
    }
  } catch (const nlohmann::json::parse_error& e) {
    detectedText.push_back("JSON_PARSE_ERROR: " + std::string(e.what()));
  }

  return detectedText;
}
