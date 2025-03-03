#include "../include/ModelHandler.h"

ModelHandler::ModelHandler(zmqpp::context& context)
    : textClassifier(context,
                     "ipc:///tmp/text_classifier_endpoint",
                     "ipc:///tmp/python_server_endpoint"),
      logger("model_handler.txt") {}

/**
 * Run both object classification and text extraction to attempt to identify the item.
 *
 * Input:
 * @param imagePath path to the image you wish to extract text from
 * @return whether successful or not
 */
bool ModelHandler::classifyObject(const std::filesystem::path& imagePath) {
  //  bool objectIdentified = this->objectClassifier.handleClassification(imagePath);
  // if (!objectIdentified) {
  // try text extraction for classification
  //   objectIdentified = this->textClassifier.handleClassification(imagePath);
  this->logger.log("Running text classificiation");
  this->textClassifier.handleClassification(imagePath);
  this->logger.log("Text classification complete");

  //}
  // return objectIdentified;
  return true;
}
