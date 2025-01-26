#include "categorizeObjects.h"
#include "../../third_party/darknet/include/darknet.h"
#include <fstream>
#include <iostream>
#include <limits.h>
#include <map>
#include <string>
#include <unistd.h>
#include <vector>
/**
 * Analyze an image using YOLO and return detected objects as a string.
 *
 * @param imagePath Path to the image file to analyze.
 * @param configPath Path to the YOLO configuration file (e.g., yolov4.cfg).
 * @param weightsPath Path to the YOLO weights file (e.g., yolov4.weights).
 * @param namesPath Path to the class names file (e.g., coco.names).
 * @return A string containing the detected objects and their confidence levels.
 */
std::vector<std::string> loadClassLabels(const std::string);

std::string analyzeImage(const std::string imagePath,
                         const std::string configPath,
                         const std::string weightsPath,
                         const std::string namesPath) {
  suppressOutput();
  // Load the YOLO model
  network* net = load_network((char*)configPath.c_str(), (char*)weightsPath.c_str(), 0);
  set_batch_network(net, 1);

  // Load the image
  image im = load_image_color((char*)imagePath.c_str(), 0, 0);
  if (!im.data) {
    return "Failed to load image: " + std::string(imagePath);
  }

  // Resize the image to match YOLO input dimensions
  image sized = letterbox_image(im, net->w, net->h);

  // Perform detection
  float* X = sized.data;
  network_predict(*net, X);

  int nboxes        = 0;
  float thresh      = 0.66; // Detection threshold
  float hier_thresh = 0.5;  // Hierarchical threshold
  detection* dets =
      get_network_boxes(net, im.w, im.h, thresh, hier_thresh, 0, 1, &nboxes, 1);

  restoreOutput();

  // Load class names
  std::vector<std::string> classNames = loadClassLabels(namesPath);

  std::string result;
  std::map<std::string, float> maxConfidence; // Store max confidence for each class

  for (int i = 0; i < nboxes; i++) {
    for (int j = 0; j < net->layers[net->n - 1].classes; j++) {
      if (dets[i].prob[j] > thresh) {
        std::string className = classNames[j];
        float confidence      = dets[i].prob[j] * 100;

        // Update max confidence for this class
        if (maxConfidence.find(className) == maxConfidence.end() ||
            maxConfidence[className] < confidence) {
          maxConfidence[className] = confidence;
        }
      }
    }
  }

  // Build the final result string
  for (const auto& pair : maxConfidence) {
    result += pair.first + " (" + std::to_string(pair.second) + "%), ";
  }

  // Cleanup
  free_detections(dets, nboxes);
  free_image(im);
  free_image(sized);
  free_network(*net);

  // Remove the trailing comma and space, if any
  if (!result.empty()) {
    result.pop_back(); // Remove last space
    result.pop_back(); // Remove last comma
  }

  return result.empty() ? "No objects detected" : result;
}

std::vector<std::string> loadClassLabels(const std::string namesPath) {
  std::vector<std::string> classNames;
  std::ifstream file(namesPath);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open class names file: " +
                             std::string(namesPath));
  }

  std::string line;
  while (std::getline(file, line)) {
    classNames.push_back(line);
  }
  file.close();
  return classNames;
}

// Function to suppress stdout and stderr
void suppressOutput() {
  fflush(stdout);
  fflush(stderr);
  freopen("/dev/null", "w", stdout);
  freopen("/dev/null", "w", stderr);
}

// Function to restore stdout and stderr
void restoreOutput() {
  fflush(stdout);
  fflush(stderr);
  freopen("/dev/tty", "w", stdout);
  freopen("/dev/tty", "w", stderr);
}
