
#include "../../logger.h"
#include "../include/visionMain.h"

/**
 * Entry into the vision code. Only called from main after vision child process is
 * forked.
 *
 * @param context The zeroMQ context with which to creates with
 * @param externalEndpoints Endpoints to the main components of the system (vision,
 * hardware, and display)
 * @return None
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

  // Wait 5 sec to ensure Python server starts
  sleep(5);

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
        LOG(FATAL) << e.what();
      }
    }
    processor.setFoodItem(foodItem);
    processor.process();
  }
}

/**
 * Start the python server for hosting models.
 *
 * @param logger Logger being used to log visionMain
 * @return True if python server was started successfully. False if failed to start python
 * server
 */
bool startPythonServer(Logger& logger) {
  pid_t pid = fork();
  logger.log("Entering startPythonServer");

  if (pid == -1) {
    logger.log("Failed to fork process");
    return false;
  }
  // Child process
  if (pid == 0) {
    logger.log("Starting python server");
    char* args[] = {(char*)"./models-venv/bin/python3",
                    (char*)"../vision/Models/server.py", nullptr};
    execvp(args[0], args);

    // Exit child process if execvp fails
    logger.log("Failed to start python server");
    exit(1);
  }
  // Parent process
  else {
    logger.log("Python server started with PID: " + std::to_string(pid));
    logger.log("Leaving startPythonServer");
    return true;
  }
}
