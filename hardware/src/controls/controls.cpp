#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <string>
#include <unistd.h>

#include "../../../food_item.h"
#include "../../../pipes.h"
#include "../io.h"
#include "cameras.h"
#include "motor.h"
#include "weight.h"

/**
 * Rotates platform in 45-degree increments, captures images, and sends the
 * data to the AI Vision system until a full 360-degree rotation is complete.
 * The process can be stopped early if AI Vision sends a "STOP" signal.ADJ_FREQUENCY
 *
 * @return None
 */
void rotateAndCapture() {
  for (int angle = 0; angle < 8; angle++) {
    LOG(INFO) << "Capturing at position " << angle;
    /**
     * char filename[] = "IMAGE_DIR + std::to_string(angle) + "_S.jpg --nopreview";
     * takePic(filename);
     * cout << "Captured image at position " << angle << endl;
     */
    takePhotos(angle);
    sendDataToVision(IMAGE_DIR, weight);

    LOG(INFO) << "Rotating platform...";
    rotateMotor();
    usleep(2);

    char stop_signal[10];
    if (read(Pipes.fromVision[READ], stop_signal, sizeof(stop_signal)) > 0) {
      LOG(INFO) << "AI Vision identified item. Stopping process.";
      break;
    }
  }
}

/**
 * This is a placeholder function for the redoThis function in hardware_entry.cpp.
 */
void redoThis(struct Pipes pipes) {
  LOG(INFO) << "Checking weight";
  /**
   * Function call to scale
   * 0 - nothing on scale
   * 1 - valid input, begin scanning
   */

  LOG(INFO) << "Beginning scan";
  /**
   * Function call to controls routine
   * has a pipe read from vision in loop
   */

  LOG(INFO) << "Sending Images from Hardware to Vision";
  // sendImagesWithinDirectory(pipes.toVision[WRITE], "../images/");

  struct FoodItem foodItem;
  foodItem.photoPath = "../images/apple.jpg";
  foodItem.name      = "Apple";
  const std::chrono::time_point now{std::chrono::system_clock::now()};
  foodItem.scanDate = std::chrono::floor<std::chrono::days>(now);

  foodItem.expirationDate = std::chrono::floor<std::chrono::days>(now);

  foodItem.catagory = "fruit";
  foodItem.weight   = 10.0;
  foodItem.quantity = 2;

  sendFoodItem(foodItem, pipes.hardwareToVision[WRITE]);
  LOG(INFO) << "Done Sending Images from Hardware to Vision";
}
