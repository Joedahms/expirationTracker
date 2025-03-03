#include "../include/IModel.h"
/**
 * Send request to models
 *
 * @param taskType Which model do you want to access
 * @param imagePath path to the image to look at
 * @return success of the attempt
 */
void IModel::sendRequest(const TaskType& taskType,
                         const std::filesystem::path& imagePath) {
  this->logger.log("Enter sendRequest");
  /*
  std::ofstream pipe_out(PIPE_IN);
  if (!pipe_out) {
    LOG(FATAL) << "Error opening pipe for writing.";
    return;
  }
  pipe_out << taskTypeToString(taskType) << " " << imagePath.string() << std::endl;
  pipe_out.close();
  */
  std::string request = taskTypeToString(taskType) + " " + imagePath.string();
  this->logger.log("Sending request: " + request);

  std::string response;
  this->requestSocket.send(request);
  this->requestSocket.receive(response);

  this->logger.log("Got response from server: " + response);
  this->logger.log("Exiting sendRequest");
}

/**
 * Wait for response from models
 *
 * @return string read back from model
 */
std::string IModel::readResponse() const {
  this->logger.log("Reading response");
  std::ifstream pipe_in(PIPE_OUT);
  if (!pipe_in) {
    LOG(FATAL) << "Error opening pipe for reading.";
    return "ERROR: Failed to read response";
  }

  std::string response;
  std::getline(pipe_in, response);
  pipe_in.close();

  this->logger.log("Response read");
  return response;
}

std::string IModel::taskTypeToString(const TaskType& taskType) const {
  this->logger.log("Converting task type to string");
  switch (taskType) {
  case CLS:
    return "CLS";
  case OCR:
    return "OCR";
  case EXP:
    return "EXP";
  case unknown:
  default:
    return "unknown";
  }
}
