#include "../include/TextClassifier.h"

TextClassifier::TextClassifier(zmqpp::context& context,
                               const std::string& textClassifierEndpoint,
                               const std::string& pythonServerEndpoint)
    : IModel("text_classifer.txt", context) {
  try {
    this->requestSocket.connect(pythonServerEndpoint);
    this->replySocket.bind(textClassifierEndpoint);
  } catch (const zmqpp::exception& e) {
    std::cerr << e.what();
  }
}

/**
 * Handle classification via text extraction
 *
 * Input:
 * @param imagePath path to the image you wish to extract text from
 * @return success of the attempt
 */
bool TextClassifier::handleClassification(const std::filesystem::path& imagePath) {
  this->logger.log("Entering handleClassification, running model");
  auto result = this->runModel(imagePath);

  std::string detectedClass = std::string(result);
  if (detectedClass.find("CLASSIFICATION") != std::string::npos) {
    this->foodItem.setName(removePrefix(detectedClass, "CLASSIFICATION: "));
    this->foodItem.setImagePath(std::filesystem::absolute(imagePath));
    this->foodItem.setCategory(FoodCategories::packaged);
    this->foodItem.setQuantity(1);
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
std::string TextClassifier::runModel(const std::filesystem::path& imagePath) {
  this->logger.log("Entering runModel");

  sendRequest(TaskType::OCR, imagePath);

  std::string result = readResponse();

  if (result.find("ERROR") != std::string::npos) {
    LOG(FATAL) << result;
  }
  this->logger.log("Result from running model: " + result);
  return result;
}
