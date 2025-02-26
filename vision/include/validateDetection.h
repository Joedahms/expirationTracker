#ifndef VALIDATE_DETECTION_H
#define VALIDATE_DETECTION_H

#include <chrono>
#include <glog/logging.h>
#include <map>
#include <string>
#include <unordered_set>

enum class TextValidationResult {
  NOT_VALID,
  POSSIBLE_CLASSIFICATION,
  POSSIBLE_EXPIRATION_DATE
};

// Function to check if a predicted class is a valid food item
bool isValidClassification(int);
TextValidationResult isValidText(
    const std::string&, bool&, bool&, std::string&, std::string&);
std::pair<bool, std::string> isTextClass(const std::string&);
bool isExpirationDate(const std::string&);
std::chrono::year_month_day parseExpirationDate(const std::string&);

#endif