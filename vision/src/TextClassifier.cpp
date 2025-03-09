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
  // Load image
  this->logger.log("Loading image");
  cv::Mat image = cv::imread(imagePath);
  if (image.empty()) {
    LOG(FATAL) << "Error: Could not load image.";
    return "SHIIIIII";
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
    std::string extractedText;
    response.get(extractedText, 0);
    this->logger.log("Received raw OCR result:" + extractedText);
    std::string foodClassification = "";
    std::string expirationDate     = "";
    try {
      nlohmann::json formatText = nlohmann::json::parse(extractedText);
      foodClassification        = formatText["Food Labels"];
      expirationDate            = formatText["Expiration Date"];
    } catch (nlohmann::json::parse_error& e) {
      std::cerr << "JSON Parse Error: " << e.what() << std::endl;
    }

    this->logger.log("Received classification result: " + foodClassification);
    this->logger.log("Received expiration date result: " + expirationDate);

    return foodLabel;
  } catch (const zmqpp::exception& e) {
    LOG(FATAL) << "ZeroMQ error: " << e.what();
    return "ZMQ_ERROR";
  }
}
