#include <filesystem>
#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <string>

#include "categorizeObjects.h"
#include "vision_pipe.h"
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

  int imageCount = 0;

  // Open a file to save the received JPEG data
  std::string outputFileName =
      outputDir + "received_image_" + std::to_string(imageCount) + ".jpg";
  FILE* receivedImage = fopen(outputFileName.c_str(), "wb");
  if (!receivedImage) {
    LOG(FATAL) << "Failed to open output file for JPEG";
    return;
  }

  // Read data from the pipe and write to the file
  char bufferImage[4096]; // Buffer for reading pipe chunks
  ssize_t bytesRead;
  while ((bytesRead = read(pipes.fromHardware[READ], bufferImage, sizeof(bufferImage))) >
         0) {
    fwrite(bufferImage, 1, bytesRead, receivedImage);
  }

  if (bytesRead == -1) {
    LOG(FATAL) << "Failed to read JPEG data from pipe";
  }

  // After receiving and saving "received_image.jpg"
  std::string detections = analyzeImage(
      outputFileName, "../third_party/darknet/cfg/yolov4.cfg",
      "../third_party/darknet/yolov4.weights", "../third_party/darknet/cfg/coco.names");

  std::cout << detections << "HAHA" << std::endl;
  fclose(receivedImage);
  LOG(INFO) << "JPEG file received from Vision process";
}
