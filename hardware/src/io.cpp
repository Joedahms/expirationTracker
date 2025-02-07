#include <filesystem>
#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <string>
#include <sys/select.h>
#include "../../food_item.h"
#include "hardware_entry.h"
#include "../../pipes.h"

/**
 * 
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