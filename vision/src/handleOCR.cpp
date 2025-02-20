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

  FILE* pipe = popen(command.c_str(), "r");
  if (!pipe) {
    return {"ERROR"};
  }

  std::ostringstream output;
  char buffer[256];
  while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
    output << buffer;
  }
  std::string result = output.str();
  pclose(pipe);

  try {
    nlohmann::json jsonData = nlohmann::json::parse(result);

    if (jsonData.contains("error")) {
      detectedText.push_back("ERROR: " + jsonData["error"].get<std::string>());
    }
    else if (jsonData.contains("text") && jsonData["text"].is_array()) {
      for (const auto& text : jsonData["text"]) {
        detectedText.push_back(text.get<std::string>());
      }
    }
    else {
      detectedText.push_back("UNEXPECTED_JSON_FORMAT");
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
  std::string detectedObject;
  std::string detectedExpDate;
  auto textDetections = runOCR(imagePath);
  for (const auto& text : textDetections) {
    switch (isValidText(text, objectDetected, expirationDateDetected, detectedObject,
                        detectedExpDate)) {
    case TextValidationResult::POSSIBLE_CLASSIFICATION:
      objectDetected        = true;
      foodItem.quantity     = 1;
      foodItem.absolutePath = std::filesystem::absolute(imagePath);
      foodItem.category     = FoodCategories::packaged;
      foodItem.name         = detectedObject;
      // Handle classification-specific logic here
      break;

    case TextValidationResult::POSSIBLE_EXPIRATION_DATE:
      expirationDateDetected  = true;
      foodItem.expirationDate = parseExpirationDate(detectedExpDate);
      // Handle expiration-specific logic here
      break;

    case TextValidationResult::NOT_VALID:
      // No action needed for invalid text
      break;
    }
  }
}
