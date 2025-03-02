
#include "../include/ObjectClassifier.h"

/**
 * Handle object classification
 *
 * Input:
 * @param imagePath path to the image you wish to extract text from
 * @return success of attempt
 */
bool ObjectClassifier::handleClassification(const std::filesystem::path& imagePath) {
  LOG(INFO) << "Handling EfficientNet Classification";
  std::string result = runModel(imagePath);

  std::string detectedClass = std::string(result);
  if (detectedClass.find("CLASSIFICATION") != std::string::npos) {
    this->foodItem.setName(removePrefix(detectedClass, "CLASSIFICATION: "));
    this->foodItem.setCategory(FoodCategories::unpackaged);
    this->foodItem.setImagePath(std::filesystem::absolute(imagePath));
    this->foodItem.setExpirationDate(std::chrono::year{2025} / std::chrono::month{2} /
                                     std::chrono::day{20});
    this->foodItem.setQuantity(1);
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

  sendRequest(TaskType::CLS, imagePath);

  std::string result = readResponse();

  if (result.find("ERROR") != std::string::npos) {
    LOG(FATAL) << result;
  }
  LOG(INFO) << result;
  return result;
}
