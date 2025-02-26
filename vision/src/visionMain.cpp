#include "../include/visionMain.h"

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
  closeUnusedPipes(pipes);

  startPythonServer();
  usleep(500000);  // Wait 0.5 sec to ensure Python server starts

  struct FoodItem foodItem;
  ImageProcessor processor = ImageProcessor(pipes, foodItem);
  // Wait for start signal from Display with 0.5sec sleep
  while (1) {
    LOG(INFO) << "Waiting for start signal from Hardware";
    if (receiveFoodItem(foodItem, pipes.hardwareToVision[READ], (struct timeval){1, 0})) {
      LOG(INFO) << "Vision Received all images from hardware";
      processor.setFoodItem(foodItem);
      processor.process();
    }
    else {
      usleep(500000);
    }
  }
}


/**
 * Start the python server for hosting models
 *
 * @return success for fail
 */
bool startPythonServer() {
  pid_t pid = fork();

  if (pid == -1) {
      LOG(FATAL) << "ERROR: Failed to fork process: " << strerror(errno);
      return false;
  }
  if (pid == 0) {  // Child process
      LOG(INFO) << "Starting Python server...";
      char *args[] = {(char *)"python3", (char *)"/path/to/server.py", nullptr};
      execvp(args[0], args);
      LOG(INFO) << "ERROR: execvp() failed to start Python server.";
      exit(1);  // Exit child process if execvp fails
  } else {  // Parent process
      LOG(INFO) << "Python server started with PID: " << pid;
      return true;
  }
}
