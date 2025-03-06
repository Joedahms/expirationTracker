#include "../include/ModelHandler.h"

ModelHandler::ModelHandler(zmqpp::context& context, const ExternalEndpoints& endpoints)
    : logger("model_handler.txt"),
      textClassifier(
          context, endpoints.textClassifierEndpoint, endpoints.pythonServerEndpoint) {}

/**
 * Run text extraction to attempt to identify the item. Upon success, update the given
 * FoodItem
 *
 * Input:
 * @param imagePath Path to the image to extract text from
 * @param foodItem foodItem to update
 * @return Whether classification was successful or not
 */
bool ModelHandler::classifyObject(const std::filesystem::path& imagePath,
                                  FoodItem& foodItem) {
  this->logger.log("Running text classificiation");
  auto result = this->textClassifier.handleClassification(imagePath);
  this->logger.log("Text classification complete");

  // update foodItem
  if (result) {
    foodItem.setName(result.value());
    foodItem.setImagePath(std::filesystem::absolute(imagePath));
    foodItem.setCategory(FoodCategories::packaged);
    foodItem.setQuantity(1);
    foodItem.setExpirationDate(std::chrono::year_month_day{
        std::chrono::year{2025}, std::chrono::month{3}, std::chrono::day{15}});
    return true;
  }
  return false;
}
