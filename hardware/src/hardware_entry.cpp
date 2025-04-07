#include <filesystem>
#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <string>
#include <unistd.h>

#include "../../food_item.h"
#include "../../logger.h"
#include "hardware.h"
#include "hardware_entry.h"

/**
 * Entry into the hardware code.
 * Only called from main after hardware child process is forked.
 *
 * @param context The zeroMQ context with which to creates with
 * @param externalEndpoints Endpoints to the main components of the system (vision,
 * hardware, and display)
 * @return None
 */
void hardwareEntry(zmqpp::context& context, bool usingMotor) {
  Logger logger("hardware_entry.txt");
  logger.log("Within hardware process");

  Hardware hardware(context);
  bool startSignalReceived = false;
  int startSignalTimeoutMs = 1000;

  if (usingMotor) {
    hardware.initDC();
  }
  //  TODO - Setup communication with Arduino for weight.

  while (1) {
    startSignalReceived = false;
    while (startSignalReceived == false) {
      logger.log("Waiting for start signal from display");

      startSignalReceived = hardware.checkStartSignal(startSignalTimeoutMs);
      if (startSignalReceived == false) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
      }
    }
    logger.log("Received start signal from display");
    bool scanSuccessful = hardware.startScan();

    // TODO - Add logic to tare weight sensor
    if (scanSuccessful) {
      logger.log("Scan successful");
    }
    else {
      logger.log("Scan unsuccessful");
    }
  }
}
