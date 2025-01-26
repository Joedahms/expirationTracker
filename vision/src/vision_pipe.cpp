#include <filesystem>
#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <string>

#include "categorizeObjects.h"
#include "vision_pipe.h"

void receive_images(int, const std::string&);

/**
 * Entry into the vision code. Only called from main after vision child process is
 * forked.
 *
 * Input:
 * - Pipes for vision to communicate with the other processes
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

  const std::string outputDir = "./received_images/";
  if (!std::filesystem::exists(outputDir)) {
    std::filesystem::create_directory(outputDir);
  }
  receive_images(pipes.fromHardware[READ], outputDir);
  LOG(INFO) << "Vision Received all images from hardware";

  for (const auto& entry : std::filesystem::directory_iterator(outputDir)) {
    // After receiving and saving "received_image.jpg"
    std::string detections = analyzeImage(
        entry.path(), "../third_party/darknet/cfg/yolov4.cfg",
        "../third_party/darknet/yolov4.weights", "../third_party/darknet/cfg/coco.names");

    std::cout << entry.path() << " has been identified as: " << detections << std::endl;
  }
}

void receive_images(int read_fd, const std::string& output_directory) {
  uint32_t image_size;
  int image_count = 0;

  while (true) {
    // Step 1: Read the header (image size in bytes)
    if (read(read_fd, &image_size, sizeof(image_size)) <= 0) {
      perror("Failed to read header");
      break;
    }

    // Step 2: Check for end-of-transmission signal
    if (image_size == 0) {
      std::cout << "End of transmission received." << std::endl;
      break;
    }

    // Step 3: Open an output file for the image
    std::string output_file =
        output_directory + "/image_" + std::to_string(image_count++) + ".jpg";
    std::ofstream file(output_file, std::ios::binary);
    if (!file.is_open()) {
      perror("Failed to open output file");
      continue;
    }

    // Step 4: Read the image data
    const int CHUNK_SIZE = 4096;
    char buffer[CHUNK_SIZE];
    while (image_size > 0) {
      ssize_t to_read =
          std::min(static_cast<ssize_t>(image_size), static_cast<ssize_t>(CHUNK_SIZE));
      ssize_t bytes_read = read(read_fd, buffer, to_read);

      if (bytes_read <= 0) {
        perror("Error reading from pipe");
        break;
      }

      file.write(buffer, bytes_read);
      image_size -= bytes_read;
    }

    file.close();
    std::cout << "Image saved to: " << output_file << std::endl;
  }
}
