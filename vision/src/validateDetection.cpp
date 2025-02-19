#include "../include/validateDetection.h"
#include <chrono>
#include <regex>
#include <unordered_map>
#include <vector>

/**
 * Determine if extracted text is something valid
 *
 * Input:
 * @param text string to evaluate
 * @param objectDetected bool to update if object is classified successfully
 * @param expirationDateDetected bool to update if exp date is identified successfully
 * Output: Enum listing what occured
 */
TextValidationResult isValidText(const std::string& text,
                                 bool& objectDetected,
                                 bool& expirationDateDetected) {
  if (!objectDetected && isTextClass(text)) {
    return TextValidationResult::POSSIBLE_CLASSIFICATION;
  }
  if (!expirationDateDetected && isExpirationDate(text)) {
    return TextValidationResult::POSSIBLE_EXPIRATION_DATE;
  }
  return TextValidationResult::NOT_VALID;
}

/**
 * Determine if text is valid class
 *
 * Input:
 * @param text string to evaluate
 * Output: Bool if is valid class
 */
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

/**
 * Determine if a given text string is an expiration date
 *
 * Date labels (Expires, Best By, Sell By, Use By) are optional.
 * Supports formats:
 * MM/DD/YYYY → (\d{1,2}/\d{1,2}/\d{4})
 * MMM DD YYYY → ([A-Z]{3}\s\d{1,2},?\s\d{4})
 * MMDDYY → (\d{6})
 * YYMMDD → (\d{6})
 *
 * Input:
 * @param text Text string to examine
 * Output: Bool on whether or not input is an expiration date
 */
bool isExpirationDate(const std::string& text) {
  static const std::regex dateRegex(
      R"((\bExpires\b|\bBest\s*by\b|\bUse\s*by\b|\bSell\s*by\b)?\s*(?:(\d{1,2}/\d{1,2}/\d{4})|([A-Z]{3}\s\d{1,2},?\s\d{4})|(\d{6})|(\d{7})))",
      std::regex::icase);

  return std::regex_search(text, dateRegex);
}

/**
 * Convert text string to std::chrono::day_month_year
 *
 * Input:
 * @param text Text string to convert
 * Output: expiration date as chrono object
 */
std::chrono::year_month_day parseExpirationDate(const std::string& text) {
  static const std::regex dateRegex(
      R"((?:\bExpires\b|\bBest\s*by\b|\bUse\s*by\b|\bSell\s*by\b)?\s*"
      R"((\d{1,2})/(\d{1,2})/(\d{4})|"                    // MM/DD/YYYY
      R"([A-Z]{3}\s(\d{1,2}),?\s(\d{4})|"                 // MMM DD YYYY
      R"((\d{2})(\d{2})(\d{2})|"                          // MMDDYY
      R"((\d{2})(\d{2})(\d{2})))", // YYMMDD
      std::regex::icase);

  std::smatch match;
  std::regex_search(text, match, dateRegex);

  // MM/DD/YYYY format
  if (match[1].matched && match[2].matched && match[3].matched) {
    return std::chrono::year_month_day(std::chrono::year(std::stoi(match[3].str())),
                                       std::chrono::month(std::stoi(match[1].str())),
                                       std::chrono::day(std::stoi(match[2].str())));
  }

  // MMM DD YYYY format (e.g., FEB 17 2025)
  if (match[4].matched && match[5].matched) {
    static const std::unordered_map<std::string, int> monthMap = {
        {"JAN", 1}, {"FEB", 2}, {"MAR", 3}, {"APR", 4},  {"MAY", 5},  {"JUN", 6},
        {"JUL", 7}, {"AUG", 8}, {"SEP", 9}, {"OCT", 10}, {"NOV", 11}, {"DEC", 12}};

    return std::chrono::year_month_day(
        std::chrono::year(std::stoi(match[6].str())),
        std::chrono::month(monthMap.at(match[4].str().substr(0, 3))),
        std::chrono::day(std::stoi(match[5].str())));
  }

  // MMDDYY format (Assuming 20XX)
  if (match[7].matched && match[8].matched && match[9].matched) {
    return std::chrono::year_month_day(
        std::chrono::year(2000 + std::stoi(match[9].str())),
        std::chrono::month(std::stoi(match[7].str())),
        std::chrono::day(std::stoi(match[8].str())));
  }

  // YYMMDD format (Assuming 20XX)
  if (match[10].matched && match[11].matched && match[12].matched) {
    return std::chrono::year_month_day(
        std::chrono::year(2000 + std::stoi(match[10].str())),
        std::chrono::month(std::stoi(match[11].str())),
        std::chrono::day(std::stoi(match[12].str())));
  }

  // Should never reach this point since isExpirationDate() guarantees validity
  throw std::runtime_error("Invalid expiration date format");
}

/**
 * Determine if a given index corresponds to a valid class
 *
 * Input:
 * @param predictedClass index of the prediction from model
 * Output: bool on whether or not the index is a valid class
 */
bool isValidClassification(int predictedClass) {
  return validFoodClasses.find(predictedClass) != validFoodClasses.end();
}