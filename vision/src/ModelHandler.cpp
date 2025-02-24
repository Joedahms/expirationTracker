#include "../include/ModelHandler.h"

ModelHandler::ModelHandler() {}

/**
 * Handle object classification
 *
 * Input:
 * @param imagePath path to the image you wish to classify
 * @param foodItem food item object to update with correct information
 * @return Whether the object was classified via object classification model
 */
bool ModelHandler::handleObjectClassification(const std::filesystem::path& imagePath,
                                              struct FoodItem& foodItem) const {
  LOG(INFO) << "Entering HandleObjectClassification";
  auto [detectionIndex, probability] = this->runEfficientNet(imagePath);
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

/**
 * Handle calling python script to run object classification.
 *
 * Input:
 * @param imagePath path to the image you wish to classify
 * @return Class index of the identified class (currently from imageNet) and bool of
 * success or fail
 */
std::pair<int, float> ModelHandler::runEfficientNet(
    const std::filesystem::path& imagePath) const {
  LOG(INFO) << "Entering runEfficientNet";
  std::string command = "./models-venv/bin/python3 ../vision/Models/efficientNet.py " +
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
    LOG(FATAL) << (output["error"]); // Handle error
  }

  int index         = output["index"];
  float probability = output["probability"];

  return {index, probability};
}

/**
 * Handle text extraction.
 *
 * Input:
 * @param imagePath path to the image you wish to extract text from
 * @param foodItem food item to update with collected information
 * Output: Vector of strings extracted from the image.
 */
bool ModelHandler::handleClassificationOCR(const std::filesystem::path& imagePath,
                                           struct FoodItem& foodItem) const {
  LOG(INFO) << "Entering HandleClassificationOCR";
  std::string detectedClass = this->runClassificationOCR(imagePath);

  if (detectedClass.find("ERROR") != std::string::npos ||
      detectedClass == "UNEXPECTED_JSON_FORMAT" ||
      detectedClass == "NO_VALID_CLASSIFICATION") {
    return false; // Classification failed or no valid result
  }

  foodItem.name         = detectedClass;
  foodItem.absolutePath = std::filesystem::absolute(imagePath);
  foodItem.category     = FoodCategories::packaged;
  foodItem.quantity     = 1;

  return true;
}

/**
 * Handle calling python script to run text extraction.
 *
 * Input:
 * @param imagePath path to the image you wish to extract text from
 * @return valid class detected
 */
std::string ModelHandler::runClassificationOCR(
    const std::filesystem::path& imagePath) const {
  LOG(INFO) << "Entering runClassificationOCR";
  std::string detectedClass;
  std::string command =
      "./models-venv/bin/python3 ../vision/Models/easyOCR.py " + imagePath.string();

  FILE* pipe = popen(command.c_str(), "r");
  if (!pipe) {
    return "ERROR";
  }

  std::ostringstream output;
  char buffer[256];
  while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
    output << buffer;
  }
  std::string result = output.str();
  std::cout << result << std::endl;
  pclose(pipe);

  try {
    nlohmann::json jsonData = nlohmann::json::parse(result);

    if (jsonData.contains("error")) {
      return "ERROR: " + jsonData["error"].get<std::string>();
    }
    else if (jsonData.contains("text") && jsonData["text"].is_array() &&
             !jsonData["text"].empty()) {
      if (jsonData["text"][0].contains("value") &&
          jsonData["text"][0]["value"].is_string()) {
        return jsonData["text"][0]["value"].get<std::string>();
      }
    }
    else {
      return "UNEXPECTED_JSON_FORMAT";
    }
  } catch (const nlohmann::json::parse_error& e) {
    return "JSON_PARSE_ERROR: " + std::string(e.what());
  }

  return "NO_VALID_CLASSIFICATION";
}