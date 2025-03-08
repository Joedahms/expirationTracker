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
    this->requestSocket.connect(SERVER_ADDRESS);
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
  cv::imencode(".jpg", image, encoded_image);
  this->logger.log("Image Compressed");

  // Get image size
  uint64_t img_size = encoded_image.size();

  // Create ZeroMQ message
  zmqpp::message message;
  zmqpp::message response;
  std::string classID;
  message << img_size;                             // First frame: image size
  message.add_raw(encoded_image.data(), img_size); // Second frame: image bytes

  // Send message
  this->logger.log("Sending image to model");
  this->requestSocket.send(message);
  this->logger.log("Image successfully sent to model. Bytes: " + img_size);
  this->requestSocket.receive(response);
  response.get(classID, 0);
  this->logger.log("Received class back from model: " + classID);

  return classID;
}
