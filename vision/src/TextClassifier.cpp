#include "../include/TextClassifier.h"

/**
 * @param context The zeroMQ context with which to creates with
 * @param textClassifierEndpoint Endpoint of the text classifier
 * @param pythonServerEndpoint Endpont of the python server
 */
TextClassifier::TextClassifier(zmqpp::context& context,
                               const std::string& textClassifierEndpoint,
                               const std::string& pythonServerEndpoint)
    : IModel("text_classifer.txt", context) {
  try {
    this->requestSocket.connect(pythonServerEndpoint);
    this->replySocket.bind(textClassifierEndpoint);
  } catch (const zmqpp::exception& e) {
    LOG(FATAL) << e.what();
  }
}

/**
 * Handle classification via text extraction.
 *
 * @param imagePath Path to the image to extract text from
 * @return pair <item identification success/fail, string>
 */
std::optional<std::string> TextClassifier::handleClassification(
    const std::filesystem::path& imagePath) {
  this->logger.log("Entering handleClassification, running model");
  auto result = this->runModel(imagePath);

  std::string detectedClass = std::string(result);
  if (detectedClass.find("CLASSIFICATION") != std::string::npos) {
    return removePrefix(detectedClass, "CLASSIFICATION: ");
  }
  return std::nullopt;
}

/**
 * Handle calling python script to run text extraction.
 *
 * Input:
 * @param imagePath path to the image to extract text from
 * @return The result send back from the python server
 */
std::string TextClassifier::runModel(const std::filesystem::path& imagePath) {
  this->logger.log("Entering runModel");

  std::string result = sendRequest(TaskType::OCR, imagePath);

  if (result.find("ERROR") != std::string::npos) {
    LOG(FATAL) << result;
  }
  this->logger.log("Result from running model: " + result);
  return result;
}
