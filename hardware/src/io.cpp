#include "../../food_item.h"
#include "../../pipes.h"
#include "hardware_entry.h"
#include <filesystem>
#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <string>
#include <sys/select.h>

/**
 * Checks to see if there is any data fromDisplay[READ]
 * Used to initialize control system and begin scanning
 *
 * Input:
 * @param pipeToRead Pipe to read signal from Display button press
 *
 * Output:
 * @return Boolean; true or false
 */
bool receivedStartSignal(int pipeToRead) {
  fd_set readPipeSet;

  FD_ZERO(&readPipeSet);
  FD_SET(pipeToRead, &readPipeSet);

  // Check pipe for data
  int startSignal = select(pipeToRead + 1, &readPipeSet, NULL, NULL, NULL);

  if (startSignal == 0) {
    LOG(INFO) << "Received start signal from display";
    return true;
  }
  else {
    LOG(INFO) << "No start signal received: ";
    return false;
  }
}