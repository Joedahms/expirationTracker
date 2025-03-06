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
void visionEntry(zmqpp::context& context, const ExternalEndpoints& externalEndpoints) {
  Logger logger("vision_entry.txt");
  logger.log("Within vision process");

  attemptStartPythonServer(logger);

  ImageProcessor processor(context, externalEndpoints);

  zmqpp::socket replySocket(context, zmqpp::socket_type::reply);
  replySocket.bind(externalEndpoints.visionEndpoint);

  zmqpp::poller poller;
  poller.add(replySocket);

  while (1) {
    FoodItem foodItem;
    logger.log("Waiting for start signal from Hardware");

    while (!startSignalCheck(replySocket, logger, foodItem, poller))
      ;
    processor.setFoodItem(foodItem);
    processor.process();
  }
}

/**
 * polls for 1000ms for socket activity and then checks replySocket for input
 *
 * @param replySocket replySocket to check for input on
 * @param logger Logger being used to log visionMain
 * @param foodItem foodItem to return with data
 * @param poller poller to use for polling socket activity
 * @return whether start signal was received or not
 */
bool startSignalCheck(zmqpp::socket& replySocket,
                      const Logger& logger,
                      FoodItem& foodItem,
                      zmqpp::poller& poller) {
  try {
    if (poller.poll(1000)) {
      if (poller.has_input(replySocket)) {
        receiveFoodItem(replySocket, "got it", foodItem);
        logger.log("Received start signal from hardware");
        return true;
      }
      return false;
    }
    else {
      logger.log("Did not receive start signal from hardware");
      return false;
    }
  } catch (const zmqpp::exception& e) {
    LOG(FATAL) << e.what();
    return false;
  }
}

/**
 * Attempts to start the python server MAX_SERVER_RETRIES times before erroring out
 *
 * @param logger Logger being used to log visionMain
 */
void attemptStartPythonServer(const Logger& logger) {
  for (int retry = 1; retry <= MAX_SERVER_RETRIES; ++retry) {
    if (startPythonServer(logger)) {
      return;
    }

    logger.log("Python server failed to start. Retrying in 2 seconds... (" +
               std::to_string(retry) + "/" + std::to_string(MAX_SERVER_RETRIES) + ")");
    sleep(2);
  }

  std::cerr << "ERROR: Failed to start Python server after " << MAX_SERVER_RETRIES
            << " attempts.\n";
}

/**
 * Start the python server for hosting models.
 *
 * @param logger Logger being used to log visionMain
 * @return True if python server was started successfully. False if failed to start python
 * server
 */
bool startPythonServer(const Logger& logger) {
  logger.log("Entering startPythonServer");
  pid_t pid = fork();

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
    perror("execvp failed");
    logger.log("Failed to start python server");
    exit(EXIT_FAILURE);
  }
  // Parent process
  else {
    logger.log("Python server started with PID: " + std::to_string(pid));
    logger.log("Leaving startPythonServer");
    return true;
  }
}
