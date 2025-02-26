#include "../include/TextClassifier.h"

/**
 * Handle classification via text extraction
 *
 * Input:
 * @param imagePath path to the image you wish to extract text from
 * @return success of the attempt
 */
bool TextClassifier::handleClassification(const std::filesystem::path& imagePath) const {
  LOG(INFO) << "Entering HandleClassificationOCR";
  auto result = this->runModel(imagePath);

  std::string detectedClass = std::string(result);
  if (detectedClass.find("CLASSIFICATION") != std::string::npos) {
    foodItem.name         = detectedClass;
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
  std::string command =
      "./models-venv/bin/python3 ../vision/Models/easyOCR.py " + imagePath.string();

  FILE* pipe = popen(command.c_str(), "r");
  if (!pipe) {
    return "ERROR: Failed to open pipe";
  }

  std::ostringstream output;
  char buffer[256];
  while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
    output << buffer;
  }
  pclose(pipe);

  std::string result = output.str();

  if (result.find("ERROR") != std::string::npos) {
    LOG(FATAL) << result;
  }
  LOG(INFO) << result;
  return result;
}
