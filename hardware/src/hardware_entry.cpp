#include <filesystem>
#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <string>
#include <unistd.h>

#include "../../food_item.h"
#include "hardware.h"
#include "hardware_entry.h"

/*
 * Entry into the hardware code.
 * Only called from main after hardware child process is forked.
 *
 * Input:
 * @param ExternalEndpoints endpoint for hardware to communicate with the other processes
 * @return None
 */
void hardwareEntry(zmqpp::context& context, struct ExternalEndpoints externalEndpoints) {
  LOG(INFO) << "Within hardware process";
  // TODO - Add a function to initialize motor and weight sensor.
  // TODO - Setup communication with Arduino for weight.

  // while 1
  Hardware hardware(context, endpoints);
  bool startSignalReceived = false;
  while (startSignalReceived == false) {
    startSignalReceived = hardware.checkStartSignal();
    if (startSignalReceived == false) {
      ;
    }
  }

  bool scanSuccessful = hardware.startScan();

  if (scanSuccessful) {
    LOG(INFO) << "Scan successful";
    // hardware.sendDataToVision();
  }
}