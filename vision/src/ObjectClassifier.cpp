
#include "../include/ObjectClassifier.h"

/**
 * Handle object classification
 *
 * Input:
 * @param imagePath path to the image you wish to extract text from
 * @return success of attempt
 */
bool ObjectClassifier::handleClassification(
    const std::filesystem::path& imagePath) const {
  LOG(INFO) << "Handling EfficientNet Classification";
  auto result = runModel(imagePath);

  if (std::holds_alternative<std::pair<int, float>>(result)) {
    auto [detectionIndex, probability] = std::get<std::pair<int, float>>(result);
    if (detectionIndex >= 0 && validFoodClasses.contains(detectionIndex)) {
      this->foodItem.name         = getFoodLabel(detectionIndex);
      this->foodItem.category     = FoodCategories::unpackaged;
      this->foodItem.absolutePath = std::filesystem::absolute(imagePath);
      this->foodItem.expirationDate =
          std::chrono::year{2025} / std::chrono::month{2} / std::chrono::day{20};
      this->foodItem.quantity = 1;
      return true;
    }
  }

  return false;
}

/**
 * Call and return output of python classification script
 *
 * Input:
 * @param imagePath path to the image you wish to extract text from
 * @return std::pair<int, float> of predicted imagenet index and probability
 */
std::variant<std::pair<int, float>, std::string> ObjectClassifier::runModel(
    const std::filesystem::path& imagePath) const {
  LOG(INFO) << "Running EfficientNet model.";
  std::string command =
      "./models-venv/bin/python3 ../vision/Models/efficientNet.py " + imagePath.string();

  FILE* pipe = popen(command.c_str(), "r");
  if (!pipe) {
    LOG(FATAL) << "Failed to open pipe.";
    return "ERROR";
  }

  std::ostringstream result;
  char buffer[256];
  while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
    result << buffer;
  }
  pclose(pipe);

  try {
    nlohmann::json output = nlohmann::json::parse(result.str());
    if (output.contains("error")) {
      LOG(FATAL) << output["error"];
      return "ERROR";
    }

    return std::make_pair(output["index"].get<int>(), output["probability"].get<float>());
  } catch (const nlohmann::json::parse_error&) {
    return "JSON_PARSE_ERROR";
  }
}
