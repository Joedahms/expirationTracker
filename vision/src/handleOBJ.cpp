#include "../include/handleOBJ.h"
#include "../include/validateDetection.h"
#include <glog/logging.h>
#include <nlohmann/json.hpp>

/**
 * Method to handle calling python script to run object classification.
 *
 * Input:
 * @param imagePath path to the image you wish to classify
 * Output: Returns the class index of the identified class (currently from imageNet).
 */
std::pair<int, float> runEfficientNet(const std::filesystem::path& imagePath) {
  std::string command = "TF_CPP_MIN_LOG_LEVEL=3 $HOME/easyocr-venv/bin/python3 "
                        "../vision/Models/efficientNet.py " +
                        imagePath.string() + " 2>/dev/null";

  std::ostringstream result;
  char buffer[256];

  FILE* pipe = popen(command.c_str(), "r");
  if (!pipe) {
    LOG(FATAL) << "Failed to open pipe to python script call.";
  }

  while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
    result << buffer;
  }
  pclose(pipe);

  // Parse JSON output
  nlohmann::json output = nlohmann::json::parse(result.str());

  if (output.contains("error")) {
    throw std::runtime_error(output["error"]); // Handle error
  }

  int index         = output["index"];
  float probability = output["probability"];

  return {index, probability};
}

/**
 * Method to handle object classification
 *
 * Input:
 * @param imagePath path to the image you wish to classify
 * @param foodItem food item object to update with correct information
 * Output: Returns the class index of the identified class (currently from imageNet).
 */
bool handleObjectClassification(const std::filesystem::path& imagePath,
                                struct FoodItem& foodItem) {
  auto [detectionIndex, probability] = runEfficientNet(imagePath);
  if (isValidClassification(detectionIndex) && probability > .5) {
    foodItem.name     = getFoodLabel(detectionIndex);
    foodItem.category = FoodCategories::unpackaged;
    foodItem.absolutePath =
        std::filesystem::absolute(imagePath); // path of photo that classified object
    foodItem.expirationDate =
        std::chrono::year{2025} / std::chrono::month{2} /
        std::chrono::day{
            20}; // hardcode expiration for now. This will have to be pulled from some
    // sort of database or estimated based on the given class.
    foodItem.quantity = 1; // default value, in my head this will be updated
    // within the display process
    return true;
  }
  return false;
}