#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <string>
#include <unistd.h>

#include "../../food_item.h"
#include "../../pipes.h"
#include "../src/hardware_entry.h"
#include "../src/io.h"
// #include "cameras.h"
#include "motor.h"
// #include "weight.h"

/**
 * Rotates platform in 45-degree increments, captures images, and sends the
 * data to the AI Vision system until a full 360-degree rotation is complete.
 * The process can be stopped early if AI Vision sends a "STOP" signal
 * 0 - continue or 1 - stop
 *
 * @return None
 */
void rotateAndCapture(struct Pipes pipes, float weight) {
  for (int angle = 0; angle < 8; angle++) {
    LOG(INFO) << "At " << angle;
    // takePhotos(angle);    // This function is for both cameras

    // if (takePhoto(angle) != angle) {
    //   LOG(INFO) << "Error taking photo";
    // }; // This function is for one camera/testing

    if (angle == 0) {
      sendDataToVision(pipes.displayToVision[WRITE], weight);
    }

    LOG(INFO) << "Rotating platform...";
    //    rotateMotor();
    usleep(200);

    bool stopSignal = false;
    if (read(pipes.visionToHardware[READ], &stopSignal, sizeof(stopSignal)) > 0 &&
        stopSignal) {
      LOG(INFO) << "AI Vision identified item. Stopping process.";
      break;
    }
    else {
      LOG(INFO) << "AI Vision did not identify item. Continuing process.";
    }
  }
}