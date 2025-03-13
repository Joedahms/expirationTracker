#include "../include/TextClassifier.h"

/**
 * @param context The zeroMQ context with which to creates with
 * @param textClassifierEndpoint Endpoint of the text classifier
 * @param pythonServerEndpoint Endpont of the python server
 */
TextClassifier::TextClassifier(zmqpp::context& context,
                               const std::string& textClassifierEndpoint)
    : IModel("text_classifer.txt", context) {
  try {
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
OCRResult TextClassifier::handleClassification(const std::filesystem::path& imagePath) {
  this->logger.log("Entering handleClassification, running model");
  std::string result = this->runModel(imagePath);
  this->logger.log("Model ran, classification received. Handling output.");
  OCRResult handled = this->handleRunModelReturn(result);
  this->logger.log("Output handled. Returning OCRResult to model handler.");
}

/**
 * Handle string built from running model
 *
 * Input:
 * @param result String returned from runModel
 * @return OCRResult containing food and expiration date vectors
 */
OCRResult TextClassifier::handleRunModelReturn(const std::string& result) {
  this->logger.log("Entering handleRunModelReturn.");
  std::vector<std::string> foodVector;
  std::vector<std::string> expVector;
  OCRResult OCRresult;

  if (result.empty() || (result.find("ERROR") != std::string::npos)) {
    this->logger.log("Received empty or Error in runModel: " + result);
    LOG(FATAL) << "Received error from runModel.";
    return OCRresult;
  }

  std::stringstream ss(result);
  std::string token;

  // Check if expiration date is present
  size_t expPos = result.find(", Exp:");
  if (expPos != std::string::npos) {
    this->logger.log("Expiration date result received. Extracting...");
    // Extract expiration date part
    std::string foodPart = result.substr(0, expPos);
    std::string expPart  = result.substr(expPos + 7); // Skip ", Exp: "

    // Split food classifications (", " separated)
    std::stringstream foodStream(foodPart);
    std::string token;
    while (getline(foodStream, token, ',')) {
      if (!token.empty()) {
        foodVector.push_back(token);
      }
    }

    for (auto& food : foodVector) {
      food.erase(0, food.find_first_not_of(" ")); // Trim leading spaces
      food.erase(food.find_last_not_of(" ") + 1); // Trim trailing spaces
    }

    // Split expiration dates (", " separated)
    std::stringstream expStream(expPart);
    while (getline(expStream, token, ',')) {
      if (!token.empty()) {
        expVector.push_back(token);
      }
    }

    for (auto& exp : expVector) {
      exp.erase(0, exp.find_first_not_of(" ")); // Trim leading spaces
      exp.erase(exp.find_last_not_of(" ") + 1); // Trim trailing spaces
    }
  }
  else {
    this->logger.log("No expiration date received. Extracting food only...");
    // No expiration date, only food classification
    std::stringstream foodStream(result);
    std::string token;
    while (getline(foodStream, token, ',')) {
      if (!token.empty()) {
        foodVector.push_back(token);
      }
    }

    for (auto& food : foodVector) {
      food.erase(0, food.find_first_not_of(" ")); // Trim leading spaces
      food.erase(food.find_last_not_of(" ") + 1); // Trim trailing spaces
    }
  }

  OCRresult.setFoodItems(foodVector);
  OCRresult.setExpirationDates(expVector);
  this->logger.log("Model output handled return result to classification handler.");
  return OCRresult;
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
  // Load image
  this->logger.log("Loading image");
  cv::Mat image = cv::imread(imagePath);
  if (image.empty()) {
    this->logger.log("Error loading image" + imagePath.string());
    LOG(FATAL) << "Error: Could not load image.";
    return "ERROR";
  }
  this->logger.log("Image loaded");
  // Encode image as JPG
  this->logger.log("Compressing image");
  std::vector<uchar> encoded_image;
  if (!cv::imencode(".jpg", image, encoded_image)) {
    LOG(FATAL) << "Error: Image encoding failed.";
    return "IMAGE_ENCODE_ERROR";
  }
  this->logger.log("Image Compressed");

  // Get image size
  uint64_t img_size = encoded_image.size();

  // Create ZeroMQ message
  zmqpp::message message;
  message << img_size;                             // First frame: image size
  message.add_raw(encoded_image.data(), img_size); // Second frame: image bytes

  try {
    this->logger.log("Sending image to model");
    this->requestSocket.send(message);
    this->logger.log("Image sent successfully. Bytes: " + std::to_string(img_size));

    // Receive OCR result
    zmqpp::message response;
    this->requestSocket.receive(response);

    if (response.parts() == 0) {
      this->logger.log("No response received from OCR server");
      LOG(FATAL) << "No response received from OCR server";
      return "NO_RESPONSE";
    }

    std::string extractedText;
    response.get(extractedText, 0);
    this->logger.log("Received raw OCR result:" + extractedText);
    std::string foodClassification = "";
    std::string expirationDate     = "";
    try {
      if (extractedText.empty() || extractedText[0] != '{') {
        this->logger.log("Invalid JSON received: " + extractedText);
        LOG(FATAL) << "Invalid JSON received: " << extractedText;
        return "INVALID_JSON";
      }

      nlohmann::json formatText = nlohmann::json::parse(extractedText);

      if (formatText.contains("Food Labels") && formatText["Food Labels"].is_array()) {
        for (const auto& label : formatText["Food Labels"]) {
          if (!foodClassification.empty()) {
            foodClassification += ", ";
          }
          foodClassification += label.get<std::string>();
        }
      }

      if (formatText.contains("Expiration Date") &&
          formatText["Expiration Date"].is_array()) {
        std::vector<std::string> dateList =
            formatText["Expiration Date"].get<std::vector<std::string>>();

        // Join expiration dates if multiple exist
        expirationDate = "";
        for (const std::string& date : dateList) {
          if (!expirationDate.empty()) {
            expirationDate += ", ";
          }
          expirationDate += date;
        }
      }

    } catch (const nlohmann::json::parse_error& e) {
      LOG(FATAL) << "JSON Parse Error: " << e.what();
      return "JSON_PARSE_ERROR";
    } catch (const std::exception& e) {
      LOG(FATAL) << "General Error: " << e.what();
      return "GENERAL_JSON_ERROR";
    }

    this->logger.log("Received classification result: " + foodClassification);
    this->logger.log("Received expiration date result: " + expirationDate);

    if (!foodClassification.empty() && !expirationDate.empty()) {
      return foodClassification + ", Exp: " + expirationDate;
    }
    else if (!foodClassification.empty()) {
      return foodClassification;
    }
    else if (!expirationDate.empty()) {
      return "Exp: " + expirationDate;
    }
    else {
      return "";
    }
  } catch (const zmqpp::exception& e) {
    LOG(FATAL) << "ZeroMQ error: " << e.what();
    return "ZMQ_ERROR";
  }
}
