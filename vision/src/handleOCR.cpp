#include "../include/handleOCR.h"
#include "../include/validateDetection.h"
#include <glog/logging.h>
#include <iostream>
#include <nlohmann/json.hpp>

/**
 * Handle calling python script to run text extraction.
 *
 * Input:
 * @param imagePath path to the image you wish to extract text from
 * Output: Vector of strings extracted from the image.
 */
std::vector<std::string> runOCR(const std::filesystem::path& imagePath) {
  std::vector<std::string> detectedText;
  std::string command =
      "./models-venv/bin/python3 ../vision/Models/easyOCR.py " + imagePath.string();
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

/**
 * Handle text extraction.
 *
 * Input:
 * @param imagePath path to the image you wish to extract text from
 * @param foodItem food item to update with collected information
 * Output: Vector of strings extracted from the image.
 */
void handleTextExtraction(const std::filesystem::path& imagePath,
                          struct FoodItem& foodItem,
                          bool& objectDetected,
                          bool& expirationDateDetected) {
  auto textDetections = runOCR(imagePath);
  for (const auto& text : textDetections) {
    switch (isValidText(text, objectDetected, expirationDateDetected)) {
    case TextValidationResult::POSSIBLE_CLASSIFICATION:
      objectDetected        = true;
      foodItem.quantity     = 1;
      foodItem.absolutePath = std::filesystem::absolute(imagePath);
      foodItem.category     = FoodCategories::packaged;
      foodItem.name         = text;
      // Handle classification-specific logic here
      break;

    case TextValidationResult::POSSIBLE_EXPIRATION_DATE:
      expirationDateDetected  = true;
      foodItem.expirationDate = parseExpirationDate(text);
      // Handle expiration-specific logic here
      break;

    case TextValidationResult::NOT_VALID:
      // No action needed for invalid text
      break;
    }
  }
}
