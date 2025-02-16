#include <filesystem>
#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <memory>

#include "../../food_item.h"
#include "categorizeObjects.h"
#include "handleOCR.h"
#include "vision_pipe.h"

/**
 * Entry into the vision code. Only called from main after vision child process is
 * forked.
 *
 * Input:
 * @param pipes Pipes for vision to communicate with the other processes
 * Output: None
 */
void visionEntry(struct Pipes pipes) {
  LOG(INFO) << "Within vision process";

  // Close write end of read pipes
  close(pipes.displayToVision[WRITE]);
  close(pipes.hardwareToVision[WRITE]);

  // Close read end of write pipes
  close(pipes.visionToDisplay[READ]);
  close(pipes.visionToHardware[READ]);
  while (1) {
    // Wait for start signal from Display with 0.5sec sleep
    LOG(INFO) << "Waiting for start signal from Hardware";
    struct FoodItem foodItemTemplate;
    // foodItemTemplate.photopath is currently the directory of images to look at
    if (receiveFoodItem(foodItemTemplate, pipes.hardwareToVision[READ])) {
      LOG(INFO) << "Vision Received all images from hardware";
      processFoodItems(pipes, foodItemTemplate.photoPath, foodItemTemplate);
    }
    else {
      usleep(500000);
    }
  }
}

void processFoodItems(struct Pipes pipes,
                      std::string imageDirectory,
                      struct FoodItem detectedFoodItem) {
  LOG(INFO) << "Vision analyzing all images";
  std::cout << "Analyzing..." << std::endl;
  std::vector<std::string> detections = analyzeImages(imageDirectory);
  LOG(INFO) << "Vision successfully analyzed all images";
  for (const auto& detection : detections) {
    std::cout << detection << std::endl;
  }
  detectedFoodItem.name = "RABBIT";
  sendFoodItem(detectedFoodItem, pipes.displayToVision[WRITE]);
}

void displayFoodItems(int pipe) {}

/**
 * Read from the given pipe and create images using the information received. Write
 * received images to the given output directory.
 *
 * Input:
 * @param ouputDirectory Directory to write information to
 * @param pipeToRead Pipe to read data from
 * Output: None
 */
void receiveImages(int pipeToRead, const std::string& ouputDirectory) {
  uint32_t image_size;
  int image_count = 0;

  while (true) {
    // Step 1: Read the header (image size in bytes)
    if (read(pipeToRead, &image_size, sizeof(image_size)) <= 0) {
      LOG(FATAL) << "Failed to read header";
      break;
    }

    // Step 2: Check for end-of-transmission signal
    if (image_size == 0) {
      LOG(INFO) << "End of transmission received";
      break;
    }

    // Step 3: Open an output file for the image
    std::string outputFile =
        ouputDirectory + "/image_" + std::to_string(image_count++) + ".jpg";
    std::ofstream file(outputFile, std::ios::binary);
    if (!file.is_open()) {
      LOG(FATAL) << "Failed to open output file";
      continue;
    }

    // Step 4: Read the image data
    const int CHUNK_SIZE = 4096;
    char buffer[CHUNK_SIZE];
    while (image_size > 0) {
      ssize_t to_read =
          std::min(static_cast<ssize_t>(image_size), static_cast<ssize_t>(CHUNK_SIZE));
      ssize_t bytes_read = read(pipeToRead, buffer, to_read);

      if (bytes_read <= 0) {
        LOG(FATAL) << "Error reading from pipe";
        break;
      }

      file.write(buffer, bytes_read);
      image_size -= bytes_read;
    }

    file.close();
    LOG(INFO) << "Image saved to: " << outputFile;
  }
}

std::vector<std::string> analyzeImages(const std::string& imageDirectory) {
  std::vector<std::string> detections;
  if (!std::filesystem::exists(imageDirectory) ||
      !std::filesystem::is_directory(imageDirectory)) {
    LOG(FATAL) << "Failed to open image directory" << imageDirectory;
    return detections;
  }
  bool objectDetected = false;
  for (const auto& entry : std::filesystem::directory_iterator(imageDirectory)) {
    std::string detection = runEfficientNet(entry.path());
    detections.push_back(detection);
    if (detection != "No objects detected") {
      objectDetected = true;
    }
  }

  if (!objectDetected && !detections.empty()) {
    LOG(INFO) << "Running text extraction script";
    std::string result = runOCR(imageDirectory + "cinnamontoastbox.jpg");
    std::cout << "TEXT" << std::endl;
    std::cout << result << std::endl;
  }
  return detections;
}
