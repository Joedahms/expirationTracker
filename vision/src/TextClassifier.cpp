#include "../include/TextClassifier.h"

/**
 * Handle classification via text extraction
 *
 * Input:
 * @param imagePath path to the image you wish to extract text from
 * @return success of the attempt
 */
bool TextClassifier::handleClassification(const std::filesystem::path& imagePath) const {
  LOG(INFO) << "Entering HandleClassificationOCR";
  auto result = this->runModel(imagePath);

  if (std::holds_alternative<std::string>(result)) {
    auto detectedClass = std::get<std::string>(result);

    if (detectedClass.find("ERROR") != std::string::npos ||
        detectedClass == "UNEXPECTED_JSON_FORMAT" ||
        detectedClass == "NO_VALID_CLASSIFICATION") {
      return false; // Classification failed or no valid result
    }

    foodItem.name         = detectedClass;
    foodItem.absolutePath = std::filesystem::absolute(imagePath);
    foodItem.category     = FoodCategories::packaged;
    foodItem.quantity     = 1;

    return true;
  }

  return false;
}

/**
 * Handle calling python script to run text extraction.
 *
 * Input:
 * @param imagePath path to the image you wish to extract text from
 * @return valid class detected
 */
std::variant<std::pair<int, float>, std::string> TextClassifier::runModel(
    const std::filesystem::path& imagePath) const {
  LOG(INFO) << "Entering runClassificationOCR";
  std::string detectedClass;
  std::string command =
      "./models-venv/bin/python3 ../vision/Models/easyOCR.py " + imagePath.string();

  FILE* pipe = popen(command.c_str(), "r");
  if (!pipe) {
    return "ERROR";
  }

  std::ostringstream output;
  char buffer[256];
  while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
    output << buffer;
  }
  std::string result = output.str();
  std::cout << result << std::endl;
  pclose(pipe);

  try {
    nlohmann::json jsonData = nlohmann::json::parse(result);

    if (jsonData.contains("error")) {
      return "ERROR: " + jsonData["error"].get<std::string>();
    }
    else if (jsonData.contains("text") && jsonData["text"].is_array() &&
             !jsonData["text"].empty()) {
      if (jsonData["text"][0].contains("value") &&
          jsonData["text"][0]["value"].is_string()) {
        return jsonData["text"][0]["value"].get<std::string>();
      }
    }
    else {
      return "UNEXPECTED_JSON_FORMAT";
    }
  } catch (const nlohmann::json::parse_error& e) {
    return "JSON_PARSE_ERROR: " + std::string(e.what());
  }

  return "NO_VALID_CLASSIFICATION";
}
