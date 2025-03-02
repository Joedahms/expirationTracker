#include <filesystem>
#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <string>
#include <unistd.h>

#include "../../food_item.h"
#include "hardware.h"
#include "hardware_entry.h"
#include "io.h"

/**
 * Entry into the hardware code. Only called from main after hardware child process is
 * forked.
 *
 * Input:
 * @param pipes Pipes for hardware to communicate with the other processes
 * Output: None
 */
void hardwareEntry(zmqpp::context& context, struct ExternalEndpoints endpoints) {
  LOG(INFO) << "Within hardware process";

  // while 1
  Hardware hardware(context, endpoints);
  bool startSignalReceived = false;
  while (startSignalReceived == false) {
    startSignalReceived = hardware.checkStartSignal();
    if (startSignalReceived == false) {
      sleep(1);
    }
  }

  bool scanSuccessful = hardware.startScan();

  if (scanSuccessful) {
    LOG(INFO) << "Scan successful";
    // hardware.sendDataToVision();
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
