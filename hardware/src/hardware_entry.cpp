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

/*
 * Entry into the hardware code.
 * Only called from main after hardware child process is forked.
 *
 * @param context The zeroMQ context with which to creates with
 * @param externalEndpoints Endpoints to the main components of the system (vision,
 * hardware, and display)
 * @return None
 *
 * TODO Add an infinite loop so that more than one food item can be scanned per
 * execution.
 */
void hardwareEntry(zmqpp::context& context, struct ExternalEndpoints externalExternalEndpoints) {
  Logger logger("hardware_entry.txt");
  logger.log("Within hardware process");
  // TODO - Add a function to initialize motor and weight sensor.
  // TODO - Setup communication with Arduino for weight.

  // while 1
  Hardware hardware(context, externalEndpoints);
  bool startSignalReceived = false;
  int startSignalTimeoutMs = 1000;
  while (startSignalReceived == false) {
    startSignalReceived = hardware.checkStartSignal(startSignalTimeoutMs);
    if (startSignalReceived == false) {
      ;
    }
  }

  bool scanSuccessful = hardware.startScan();

  if (scanSuccessful) {
    logger.log("Scan successful");
  }
  else {
    logger.log("Scan unsuccessful");
  }
}
