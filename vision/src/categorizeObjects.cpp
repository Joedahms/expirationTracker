#include "categorizeObjects.h"
#include <fstream>
#include <iostream>
#include <map>
#include <opencv2/dnn.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

/**
 * Analyze an image using YOLO (via OpenCV DNN) and return detected objects as a string.
 *
 * @param imagePath Path to the image file to analyze.
 * @param configPath Path to the YOLO configuration file (e.g., yolov4-tiny.cfg).
 * @param weightsPath Path to the YOLO weights file (e.g., yolov4-tiny.weights).
 * @param namesPath Path to the class names file (e.g., coco.names).
 * @return A string containing the detected objects and their confidence levels.
 */
std::string analyzeImage(const std::string imagePath,
                         const std::string configPath,
                         const std::string weightsPath,
                         const std::string namesPath) {
  // Load class labels
  std::vector<std::string> classNames = loadClassLabels(namesPath);

  // Load the YOLO model
  cv::dnn::Net net = cv::dnn::readNetFromDarknet(configPath, weightsPath);
  net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
  net.setPreferableTarget(
      cv::dnn::DNN_TARGET_CPU); // Use DNN_TARGET_CUDA for GPU acceleration

  // Load the image
  cv::Mat image = cv::imread(imagePath);
  if (image.empty()) {
    return "Failed to load image: " + imagePath;
  }

  // Prepare the image for YOLO (resize, normalize, and create a blob)
  cv::Mat blob = cv::dnn::blobFromImage(image, 1 / 255.0, cv::Size(416, 416),
                                        cv::Scalar(), true, false);

  // Set the blob as input and run the forward pass
  net.setInput(blob);
  std::vector<cv::Mat> outputs;
  net.forward(outputs, net.getUnconnectedOutLayersNames());

  // Parse YOLO output
  float confThreshold = 0.2; // Detection confidence threshold
  float nmsThreshold  = 0.4; // Non-Max Suppression threshold
  std::vector<int> classIds;
  std::vector<float> confidences;
  std::vector<cv::Rect> boxes;

  for (const cv::Mat& output : outputs) {
    const float* data = (const float*)output.data; // Corrected to const float*
    for (int i = 0; i < output.rows; i++, data += output.cols) {
      float confidence = data[4]; // Objectness confidence
      if (confidence > confThreshold) {
        const float* scores = data + 5; // Class scores start after the first 5 elements
        cv::Point classIdPoint;         // Use cv::Point for class ID
        double maxScore;

        // Wrap scores in a cv::Mat for minMaxLoc
        cv::Mat scoresMat(1, (int)classNames.size(), CV_32F, (void*)scores);
        cv::minMaxLoc(scoresMat, nullptr, &maxScore, nullptr, &classIdPoint);

        if (maxScore > confThreshold) {
          int centerX = (int)(data[0] * image.cols);
          int centerY = (int)(data[1] * image.rows);
          int width   = (int)(data[2] * image.cols);
          int height  = (int)(data[3] * image.rows);
          int left    = centerX - width / 2;
          int top     = centerY - height / 2;

          classIds.push_back(classIdPoint.x); // Extract the class ID from Point.x
          confidences.push_back((float)maxScore);
          boxes.push_back(cv::Rect(left, top, width, height));
        }
      }
    }
  }

  // Apply Non-Max Suppression (NMS)
  std::vector<int> indices;
  cv::dnn::NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);

  std::string result;
  std::map<std::string, float> maxConfidence; // Store max confidence for each class

  for (int idx : indices) {
    std::string className = classNames[classIds[idx]];
    float confidence      = confidences[idx] * 100;
    if (maxConfidence.find(className) == maxConfidence.end() ||
        maxConfidence[className] < confidence) {
      maxConfidence[className] = confidence;
    }
  }

  // Build the final result string
  for (const auto& pair : maxConfidence) {
    result += pair.first + " (" + std::to_string(pair.second) + "%), ";
  }

  // Remove the trailing comma and space, if any
  if (!result.empty()) {
    result.pop_back(); // Remove last space
    result.pop_back(); // Remove last comma
  }

  return result.empty() ? "No objects detected" : result;
}

/**
 * Load class labels from the given path
 *
 * @param namesPath Path to the class names file (e.g., coco.names).
 * @return A vector of strings containing all class labels
 */
std::vector<std::string> loadClassLabels(const std::string namesPath) {
  std::vector<std::string> classNames;
  std::ifstream file(namesPath);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open class names file: " + namesPath);
  }

  std::string line;
  while (std::getline(file, line)) {
    classNames.push_back(line);
  }
  file.close();
  return classNames;
}
