
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
  std::string result = runModel(imagePath);

  std::string detectedClass = std::string(result);
  if (detectedClass.find("CLASSIFICATION") != std::string::npos) {
    this->foodItem.name         = detectedClass;
    this->foodItem.category     = FoodCategories::unpackaged;
    this->foodItem.absolutePath = std::filesystem::absolute(imagePath);
    this->foodItem.expirationDate =
        std::chrono::year{2025} / std::chrono::month{2} / std::chrono::day{20};
    this->foodItem.quantity = 1;
    return true;
  }
  return false;
}

/**
 * Call and return output of python classification script
 *
 * Input:
 * @param imagePath path to the image you wish to extract text from
 * @return string containing detected class
 */
std::string ObjectClassifier::runModel(const std::filesystem::path& imagePath) const {
  LOG(INFO) << "Running EfficientNet model.";
  std::string command =
      "./models-venv/bin/python3 ../vision/Models/efficientNet.py " + imagePath.string();

  FILE* pipe = popen(command.c_str(), "r");
  if (!pipe) {
    LOG(FATAL) << "Failed to open pipe.";
    return "ERROR";
  }

  std::ostringstream output;
  char buffer[256];

  while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
    output << buffer;
  }
  pclose(pipe);
  std::string result = output.str();

  if (result.find("ERROR") != std::string::npos) {
    LOG(FATAL) << result;
  }
  LOG(INFO) << result;
  return result;
}
