#include <glog/logging.h>
#include <iostream>
#include <sqlite3.h>

#include "../../food_item.h"
#include "../../pipes.h"
#include "external_handler.h"
#include "sql_food.h"

/**
 * Handle communication between the display and other external processes. Communication to
 * the SDL process is not handled in this function.
 *
 * @param pipes Display pipes used to communicate with external processes
 * @return None
 */
void externalHandler(struct Pipes pipes) {
  struct FoodItem foodItem;
  struct timeval timeout;
  timeout.tv_sec        = 1;
  timeout.tv_usec       = 0;
  bool foodItemReceived = false;
  foodItemReceived      = receiveFoodItem(foodItem, pipes.visionToDisplay[READ], timeout);
  if (foodItemReceived) {
    LOG(INFO) << "Food item received from vision";
    sqlite3* database = nullptr;
    openDatabase(&database);

    storeFoodItem(database, foodItem);

    sqlite3_close(database);
  }
}

bool sdlHandler(struct Pipes pipes, int* sdlToDisplay, int* displayToSdl) {
  bool stringFromSdl = false;

  fd_set readPipeSet;

  FD_ZERO(&readPipeSet);
  FD_SET(sdlToDisplay[READ], &readPipeSet);

  struct timeval timeout;
  timeout.tv_sec  = 1;
  timeout.tv_usec = 0;
  int pipeReady   = select(sdlToDisplay[READ] + 1, &readPipeSet, NULL, NULL, &timeout);

  if (pipeReady == -1) {
    LOG(FATAL) << "Select error when receiving food item";
  }
  else if (pipeReady == 0) { // No data available
    ;
  }
  if (FD_ISSET(sdlToDisplay[READ], &readPipeSet)) { // Data available
    LOG(INFO) << "Sending start signal to hardware";
    std::string sdlString = readString(sdlToDisplay[READ]);
    writeString(pipes.displayToHardware[WRITE], sdlString);
    stringFromSdl = true;
  }
  return stringFromSdl;
}
