#include "../include/ModelHandler.h"

ModelHandler::ModelHandler(zmqpp::context& context)
    : logger("model_handler.txt"), textClassifier(context,
                                                  "ipc:///tmp/text_classifier_endpoint",
                                                  "ipc:///tmp/python_server_endpoint") {}

/**
 * Run text extraction to attempt to identify the item.
 *
 * Input:
 * @param imagePath Path to the image to extract text from
 * @return Whether classification was successful or not
 */
bool ModelHandler::classifyObject(const std::filesystem::path& imagePath) {
  this->logger.log("Running text classificiation");
  this->textClassifier.handleClassification(imagePath);
  this->logger.log("Text classification complete");

  return true;
}
