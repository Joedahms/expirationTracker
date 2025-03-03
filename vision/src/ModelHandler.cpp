#include "../include/ModelHandler.h"

ModelHandler::ModelHandler(struct FoodItem& foodItem)
    : objectClassifier(), textClassifier(), logger("model_handler.txt") {}

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
