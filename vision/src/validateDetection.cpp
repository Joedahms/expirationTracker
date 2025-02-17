#include "../include/validateDetection.h"
#include <regex>
#include <vector>

TextValidationResult isValidText(const std::string& text) {
  if (isTextClass(text)) {
    return TextValidationResult::POSSIBLE_CLASSIFICATION;
  }
  if (isExpirationDate(text)) {
    return TextValidationResult::POSSIBLE_EXPIRATION_DATE;
  }
  return TextValidationResult::NOT_VALID;
}
bool isTextClass(const std::string& text) {
  static const std::vector<std::string> classificationKeywords = {
      "Milk",   "Cheese", "Bread", "Eggs",    "Yogurt",
      "Butter", "Juice",  "Meat",  "Chicken", "Fish"};

  for (const auto& keyword : classificationKeywords) {
    if (text.find(keyword) != std::string::npos) {
      return true;
    }
  }
  return false;
}
bool isExpirationDate(const std::string& text) {
  static const std::regex dateRegex(
      R"((\bExpires\b|\bBest\s*by\b|\bUse\s*by\b|\bSell\s*by\b).*\b(\d{1,2}/\d{1,2}/\d{4})\b)",
      std::regex::icase);

  return std::regex_search(text, dateRegex);
}

bool isValidClassification(int predictedClass) {
  return validFoodClasses.find(predictedClass) != validFoodClasses.end();
}