#include "../include/ModelHandler.h"

ModelHandler::ModelHandler(zmqpp::context& context, std::string& serverAddress)
    : logger("model_handler.txt"),
      textClassifier(context, VisionExternalEndpoints::textClassifierEndpoint) {
  this->textClassifier.connectToServer(serverAddress);
}

/**
 * Run text extraction to attempt to identify the item. Upon success, update the given
 * FoodItem
 *
 * Input:
 * @param imagePath Path to the image to extract text from
 * @param foodItem foodItem to update
 * @return Whether classification was successful or not
 */
ClassifyObjectReturn ModelHandler::classifyObject(const std::filesystem::path& imagePath,
                                                  FoodItem& foodItem) {
  this->logger.log("Entering classify object");
  ClassifyObjectReturn classifyObjectReturn{false, false};
  this->logger.log("Running text classificiation");
  OCRResult result = this->textClassifier.handleClassification(imagePath);
  this->logger.log("Text classification complete");

  // update foodItem
  if (result.hasFoodItems()) {
    this->logger.log("Food items detected. Updating FoodItem class.");
    classifyObjectReturn.foodItem = true;
    // right now just grab the first food item. Later handle weight
    std::string firstFoodItem = result.getFoodItems()[0];
    foodItem.setName(firstFoodItem);
    foodItem.setQuantity(1);
    if (knownFoodData.find(firstFoodItem) != knownFoodData.end()) {
      this->logger.log("Unpackaged item detected.");
      // classified food item is some sort of produce
      foodItem.setCategory(FoodCategories::unpackaged);
      foodItem.setExpirationDate(
          calculateExpirationDate(foodItem.getScanDate(), foodItem.getName()));
      classifyObjectReturn.expirationDate = true;
    }
    else {
      this->logger.log("Packaged item detected.");
      foodItem.setCategory(FoodCategories::packaged);
    }
  }
  else {
    this->logger.log("No food detected.");
  }
  if (result.hasExpirationDates()) {
    // OCR return expiration date in form YYYY/MM/DD
    this->logger.log("Expiration date detected. Updating FoodItem class.");
    foodItem.setExpirationDate(
        this->extractExpirationDate(result.getExpirationDates()[0]));
    classifyObjectReturn.expirationDate = true;
  }
  else {
    this->logger.log("No expiration date detected.");
  }
  this->logger.log(
      "Object classification complete. Returning result to image processor.");
  return classifyObjectReturn;
}

std::chrono::year_month_day ModelHandler::extractExpirationDate(
    const std::string& expDate) {
  this->logger.log("Entering extractExpirationDate.");
  std::istringstream iss(expDate);
  int year, month, day;
  char delim1 = '/';
  iss >> year >> delim1 >> month >> delim1 >> day;
  return {std::chrono::year{year}, std::chrono::month{month}, std::chrono::day{day}};
}

void ModelHandler::notifyServer(const std::string& notification) {
  this->textClassifier.notifyServer(notification);
}