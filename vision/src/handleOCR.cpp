#include "handleOCR.h"
#include <glog/logging.h>
#include <iostream>
#include <nlohmann/json.hpp>

/**
 * Method to handle calling python script to run text extraction.
 *
 * Input:
 * @param imagePath path to the image you wish to extract text from
 * Output: String of text extract from the image.
 */

std::string runOCR(const std::string& imagePath) {
  std::string command =
      "$HOME/easyocr-venv/bin/python3 ../vision/Models/easyOCR.py " + imagePath;
  std::string result;
  char buffer[256];

  FILE* pipe = popen(command.c_str(), "r");
  if (!pipe)
    return "ERROR";

  while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
    result += buffer;
  }
  pclose(pipe);
  return result;
}

/**
 * Method to handle calling python script to run object classification.
 *
 * Input:
 * @param imagePath path to the image you wish to classify
 * Output: Returns the class index of the identified class (currently from imageNet).
 */

std::pair<int, float> runEfficientNet(const std::string& imagePath) {
  std::string command = "TF_CPP_MIN_LOG_LEVEL=3 $HOME/easyocr-venv/bin/python3 "
                        "../vision/Models/efficientNet.py " +
                        imagePath + " 2>/dev/null";

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

  std::cout << result.str() << std::endl;

  // Parse JSON output
  nlohmann::json output = nlohmann::json::parse(result.str());

  if (output.contains("error")) {
    throw std::runtime_error(output["error"]); // Handle error
  }

  int index         = output["index"];
  float probability = output["probability"];

  return {index, probability};
}
