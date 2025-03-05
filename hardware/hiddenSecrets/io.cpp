// #include <filesystem>
// #include <fstream>
// #include <glog/logging.h>
// #include <iostream>
// #include <string>
// #include <sys/select.h>

// #include "../../food_item.h"
// #include "../../pipes.h"
// #include "hardware_entry.h"

// /**
//  * Checks to see if there is any data fromDisplay[READ]
//  * Used to initialize control system and begin scanning
//  *
//  * Input:
//  * @param pipeToRead Pipe to read signal from Display button press
//  *
//  * Output:
//  * @return Boolean; true or false
//  */

// bool receivedStartSignal(int pipeToRead) {
//   fd_set readPipeSet;

//   FD_ZERO(&readPipeSet);
//   FD_SET(pipeToRead, &readPipeSet);

//   struct timeval timeout;
//   timeout.tv_sec  = 1;
//   timeout.tv_usec = 0;

//   // Check pipe for data
//   int startSignal = select(pipeToRead + 1, &readPipeSet, NULL, NULL, &timeout);

//   if (startSignal == -1) {
//     LOG(FATAL) << "Select error when checking for start signal";
//   }
//   if (startSignal == 0) {
//     LOG(INFO) << "No start signal received";
//     return false;
//   }
//   else {
//     LOG(INFO) << "Received start signal from display";
//     std::string startSignal = readString(pipeToRead);
//     return true;
//   }
// }
