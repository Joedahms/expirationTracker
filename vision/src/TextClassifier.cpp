#include "../include/TextClassifier.h"

/**
 * Handle classification via text extraction
 *
 * Input:
 * @param imagePath path to the image you wish to extract text from
 * @return success of the attempt
 */
bool TextClassifier::handleClassification(const std::filesystem::path& imagePath) {
  LOG(INFO) << "Entering HandleClassificationOCR";
  auto result = this->runModel(imagePath);

  std::string detectedClass = std::string(result);
  if (detectedClass.find("CLASSIFICATION") != std::string::npos) {
    foodItem.name         = removePrefix(detectedClass, "CLASSIFICATION: ");
    foodItem.absolutePath = std::filesystem::absolute(imagePath);
    foodItem.category     = FoodCategories::packaged;
    foodItem.quantity     = 1;
  }
  return true;
}

/**
 * Handle calling python script to run text extraction.
 *
 * Input:
 * @param imagePath path to the image you wish to extract text from
 * @return valid class detected
 */
std::string TextClassifier::runModel(const std::filesystem::path& imagePath) const {
  LOG(INFO) << "Entering runClassificationOCR";

  sendRequest(TaskType::OCR, imagePath);

  std::string result = readResponse();

  if (result.find("ERROR") != std::string::npos) {
    LOG(FATAL) << result;
  }
  LOG(INFO) << result;
  return result;
}
