#include "hardware_entry.h"
#include "../../food_item.h"
#include "io.h"
#include <filesystem>
#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <string>
#include <unistd.h>

/**
 * Entry into the hardware code. Only called from main after hardware child process is
 * forked.
 *
 * Input:
 * @param pipes Pipes for hardware to communicate with the other processes
 * Output: None
 */
void hardwareEntry(struct Pipes pipes) {
  LOG(INFO) << "Within vision process";

  // Close write end of read pipes
  close(pipes.displayToHardware[WRITE]);
  close(pipes.visionToHardware[WRITE]);

  // Close unused ends of the pipes
  // close(pipes.fromDisplay[READ]); // Not currently used
  // close(pipes.fromVision[READ]);  // Not currently used
  // close(pipes.toDisplay[WRITE]);  // Not currently used
  // close(pipes.toVision[WRITE]);   // Not currently used

  // Wait for start signal from Display with 0.5sec sleep
  LOG(INFO) << "Waiting for start signal from Display";
  if (receivedStartSignal(pipes.fromDisplay[READ]) == 0) {
    usleep(500000);
  }
}

void redoThis(struct Pipes pipes) {
  LOG(INFO) << "Checking weight";
  /**
   * Function call to scale
   * 0 - nothing on scale
   * 1 - valid input, begin scanning
   */

  LOG(INFO) << "Beginning scan";
  /**
   * Function call to controls routine
   * has a pipe read from vision in loop
   */

  LOG(INFO) << "Sending Images from Hardware to Vision";
  // sendImagesWithinDirectory(pipes.toVision[WRITE], "../images/");

  struct FoodItem foodItem;
  foodItem.photoPath = "../images/apple.jpg";
  foodItem.name      = "Apple";
  const std::chrono::time_point now{std::chrono::system_clock::now()};
  foodItem.scanDate       = std::chrono::floor<std::chrono::days>(now);
  foodItem.expirationDate = std::chrono::floor<std::chrono::days>(now);
  foodItem.catagory       = "fruit";
  foodItem.weight         = 10.0;
  foodItem.quantity       = 2;

  sendFoodItem(foodItem, pipes.toVision[WRITE]);
  LOG(INFO) << "Done Sending Images from Hardware to Vision";
}
}

/**
 * Method to send images from a given directory using the pipe provided. It cycles through
 * all .jpg images within the directory and sends them across processes.
 *
 * Input:
 * @param pipeToWrite Pipe to write image information to
 * @param directory_path Directory path as a string to get images from
 * Output: None
 */

void sendImagesWithinDirectory(int pipeToWrite, const std::string& directory_path) {
  for (const auto& entry : std::filesystem::directory_iterator(directory_path)) {
    if (entry.is_regular_file()) {
      const std::string file_path = entry.path().string();

      // Check if the file has a .jpg extension
      if (entry.path().extension() != ".jpg") {
        continue; // Skip non-.jpg files
      }

      // Open the file
      std::ifstream file(file_path, std::ios::binary | std::ios::ate);
      if (!file.is_open()) {
        LOG(FATAL) << "Failed to open file: " << file_path;
        continue;
      }

      // Get the file size
      std::streamsize file_size = file.tellg();
      file.seekg(0, std::ios::beg);

      // Header: Send the file size (4 bytes)
      uint32_t header = static_cast<uint32_t>(file_size);
      write(pipeToWrite, &header, sizeof(header)); // Send file size

      // Send the file data in chunks
      const int CHUNK_SIZE = 4096;
      char buffer[CHUNK_SIZE];
      while (file_size > 0) {
        std::streamsize to_read =
            std::min(file_size, static_cast<std::streamsize>(CHUNK_SIZE));
        file.read(buffer, to_read);
        write(pipeToWrite, buffer, to_read);
        file_size -= to_read;
      }

      file.close();
    }
  }

  // Send a special header with size 0 to signal end of transmission
  uint32_t end_signal = 0;
  write(pipeToWrite, &end_signal, sizeof(end_signal));
}
