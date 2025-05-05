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
void hardwareEntry(zmqpp::context& context, const HardwareFlags& hardwareFlags) {
  Logger logger("hardware_entry.txt");
  logger.log("Within hardware process");

  Hardware hardware(context, hardwareFlags);

  bool startSignalReceived       = false;
  const int startSignalTimeoutMs = 1000;

  if (hardwareFlags.usingMotor) {
    hardware.initDC();
  }

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
    hardware.startScan();
  }
}
