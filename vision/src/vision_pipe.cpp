#include <filesystem>
#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <memory>

#include "../../food_item.h"
#include "categorizeObjects.h"
#include "vision_pipe.h"

/**
 * Entry into the vision code. Only called from main after vision child process is
 * forked.
 *
 * Input:
 * @param pipes Pipes for vision to communicate with the other processes
 * Output: None
 */
void visionEntry(struct VisionPipes pipes) {
  LOG(INFO) << "Within vision process";

  // Close unused ends of the pipes
  close(pipes.fromDisplay[WRITE]);
  close(pipes.fromHardware[WRITE]);
  close(pipes.toDisplay[READ]);
  close(pipes.toHardware[READ]);

  // Close not currently used ends
  // close(pipes.fromHardware[READ]); // Not currently used
  close(pipes.fromDisplay[READ]); // Not currently used
  close(pipes.toDisplay[WRITE]);  // Not currently used
  close(pipes.toHardware[WRITE]); // Not currently used

  struct FoodItem foodItem;
  receiveFoodItem(foodItem, pipes.fromHardware[READ]);
  std::cout << foodItem.photoPath << std::endl;
  std::cout << foodItem.name << std::endl;

  std::cout << static_cast<int>(foodItem.scanDate.year()) << std::endl;
  std::cout << static_cast<unsigned>(foodItem.scanDate.month()) << std::endl;
  std::cout << static_cast<unsigned>(foodItem.scanDate.day()) << std::endl;

  std::cout << static_cast<int>(foodItem.expirationDate.year()) << std::endl;
  std::cout << static_cast<unsigned>(foodItem.expirationDate.month()) << std::endl;
  std::cout << static_cast<unsigned>(foodItem.expirationDate.day()) << std::endl;

  std::cout << foodItem.catagory << std::endl;
  std::cout << foodItem.weight << std::endl;
  std::cout << foodItem.quantity << std::endl;

  // receiveImages(pipes.fromHardware[READ], outputDir);
  LOG(INFO) << "Vision Received all images from hardware";
  LOG(INFO) << "Vision analyzing all images";

  const std::string outputDir = "./received_images/";
  if (!std::filesystem::exists(outputDir)) {
    std::filesystem::create_directory(outputDir);
  }

  std::vector<std::string> detections = analyzeImages(outputDir);
  LOG(INFO) << "Vision successfully analyzed all images";
  std::cout << "The following objects were detected in the images analyzed:" << std::endl;
  for (const auto& detection : detections) {
    std::cout << detection << std::endl;
  }
}

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
  for (const auto& entry : std::filesystem::directory_iterator(imageDirectory)) {
    // After receiving and saving "received_image.jpg"
    std::string detection = analyzeImage(
        entry.path(), "../YOLO/yolov4-tiny/cfg/yolov4-tiny.cfg",
        "../YOLO/yolov4-tiny/yolov4-tiny.weights", "../YOLO/yolov4-tiny/cfg/coco.names");

    detections.push_back(detection);
  }
  return detections;
}
