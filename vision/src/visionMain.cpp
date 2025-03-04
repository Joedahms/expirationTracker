#include "../include/visionMain.h"
#include "../../logger.h"

/**
 * Entry into the vision code. Only called from main after vision child process is
 * forked.
 *
 * Input:
 * @param pipes Pipes for vision to communicate with the other processes
 * Output: None
 */
void visionEntry(zmqpp::context& context, struct ExternalEndpoints externalEndpoints) {
  Logger logger("vision_entry.txt");
  logger.log("Within vision process");

  int maxRetries = 5;
  int retryCount = 0;

  while (!startPythonServer(logger)) {
    retryCount++;

    if (retryCount >= maxRetries) {
      std::cerr << "ERROR: Failed to start Python server after " << retryCount
                << " attempts.";
    }
    logger.log("Python server failed to start. Retrying in 2 seconds... (" +
               std::to_string(retryCount) + "/" + std::to_string(maxRetries) + ")");
    sleep(2);
  }

  sleep(5); // Wait 5 sec to ensure Python server starts

  ImageProcessor processor(context, externalEndpoints);

  zmqpp::socket replySocket(context, zmqpp::socket_type::reply);
  replySocket.bind(externalEndpoints.visionEndpoint);
  while (1) {
    FoodItem foodItem;
    logger.log("Waiting for start signal from Hardware");

    bool startSignalReceived = false;
    while (startSignalReceived == false) {
      try {
        zmqpp::poller poller;
        poller.add(replySocket);

        if (poller.poll(1000)) {
          if (poller.has_input(replySocket)) {
            receiveFoodItem(replySocket, "got it", foodItem);
            logger.log("Received start signal from hardware");
            startSignalReceived = true;
          }
        }
        else {
          logger.log("Did not receive start signal from hardware");
        }
      } catch (const zmqpp::exception& e) {
        std::cerr << e.what();
      }
    }
    //  LOG(INFO) << "Vision Received all images from hardware";
    processor.setFoodItem(foodItem);
    processor.process();
  }
  // else {
  //  usleep(500000);
  //}
}

/**
 * Start the python server for hosting models
 *
 * @return success for fail
 */
bool startPythonServer(Logger& logger) {
  pid_t pid = fork();
  logger.log("Entering startPythonServer");

  if (pid == -1) {
    logger.log("Failed to fork process");
    return false;
  }
  if (pid == 0) { // Child process
    logger.log("Starting python server");
    char* args[] = {(char*)"./models-venv/bin/python3",
                    (char*)"../vision/Models/server.py", nullptr};
    execvp(args[0], args);
    logger.log("failed to start python server");
    exit(1); // Exit child process if execvp fails
  }
  else { // Parent process
         //    LOG(INFO) << "Python server started with PID: " << pid;
    logger.log("Leaving startPythonServer");
    return true;
  }
}
