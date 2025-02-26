#include "../include/validateDetection.h"
#include "../include/helperFunctions.h"
#include <chrono>
#include <iostream>
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
                                 bool& expirationDateDetected,
                                 std::string& detectedClassName,
                                 std::string& detectedExpirationDate) {
  auto detectedClass = isTextClass(text);
  if (!objectDetected && detectedClass.first) {
    detectedClassName = detectedClass.second;
    return TextValidationResult::POSSIBLE_CLASSIFICATION;
  }
  /*
  if (!expirationDateDetected && isExpirationDate(text)) {
    return TextValidationResult::POSSIBLE_EXPIRATION_DATE;
  }
  */
  return TextValidationResult::NOT_VALID;
}

/**
 * Determine if text is valid class
 *
 * Input:
 * @param text string to evaluate
 * @return pair of bool,string bool: is it a class, string: the class
 */
std::pair<bool, std::string> isTextClass(const std::string& text) {
  static const std::vector<std::string> classificationKeywords = {
      "milk",          "cheese",        "bread",      "eggs",        "yogurt",
      "butter",        "juice",         "meat",       "chicken",     "fish",
      "cereal",        "pasta",         "rice",       "flour",       "sugar",
      "salt",          "pepper",        "coffee",     "tea",         "honey",
      "jam",           "peanut butter", "chocolate",  "candy",       "cookies",
      "crackers",      "granola",       "oats",       "popcorn",     "chips",
      "pancake mix",   "syrup",         "beans",      "corn",        "tomatoes",
      "tuna",          "soup",          "fruit",      "vegetables",  "meals",
      "pizza",         "fries",         "ice cream",  "energy bars", "protein powder",
      "noodles",       "ketchup",       "mayonnaise", "mustard",     "soy sauce",
      "hot sauce",     "dressing",      "oil",        "vinegar",     "pudding",
      "whipped cream", "sour cream",    "tofu",       "bacon"};

  std::string lowerText          = toLowerCase(cleanText(text));
  std::vector<std::string> words = splitWords(lowerText);

  for (const std::string& word : words) {
    if (word.size() <= 2)
      continue; // Ignore words ≤ 2 characters

    for (const std::string& keyword : classificationKeywords) {
      std::string lowerKeyword = toLowerCase(keyword);

      if (word == lowerKeyword) {
        return {true, lowerKeyword}; // Exact match
      }
    }
  }

  return {false, ""};
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