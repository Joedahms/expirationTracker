/**
 * Author: Geromy Cunningham
 * Date: 07 APR 2025
 * HX711 Custom Scale for bilateral communication with Raspberry Pi 5
 * Some of the ideas were also taken from the SparkFun example code by Nathan Seidle
 * Setup your scale and start the sketch WITHOUT a weight on the scale
 * The scale will be tared to zero. This is the zeroFactor.
 * This code is designed to work with pounds as the unit for US food labels.
 * Your calibration factor may be very positive or very negative. It all depends on the
 * setup of your scale system and the direction the sensors deflect from zero state
 * This code is a modified version of the HX711 example code from by Bogdan Necula:
 *      "https://github.com/bogde/HX711" bogde's library is released under a
 * GNU GENERAL PUBLIC LICENSE
 */

#include "HX711.h"

#define LOADCELL_DOUT_PIN 3 // 6
#define LOADCELL_SCK_PIN  2 // 7

HX711 scale;

// 400150.00 worked for the small setup I created.
// Will need to be redone for the final "scale" created
float calibrationFactor = 420.00;
float weight            = 0.0f;
float prevWeight        = 0.0f;
float epsilon           = 0.1f;
bool setupDone          = false;
int count               = 0;
int errCount            = 0;
long zeroFactor, weightThreshold;

void setup() {
  Serial.begin(9600);
  // wait for serial port to connect. Needed for native USB port only
  while (!Serial) {
  };
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale();
  scale.tare(); // Reset the scale to 0
  // scale.set_gain(64);

  // This can be used to remove the need to tare the scale.
  // Useful in permanent scale projects.
  zeroFactor = scale.read_average(); // Get a baseline reading
  setupDone  = true;
  Serial.println(setupDone);
}

/**
 * Main function that acts as a switch for bidirectional commmunication.
 * When the process initiates, the first reading is used for a +-10% threshold.
 *
 * Pi 5 sends a:
 * @param 1 -> request for weight
 * @param 2 -> request for tare, notify of end of process
 * @param 4 -> request for process start confimation
 * @return 1 -> item weight || -1 for no weight
 * @return 2 -> 1 for confimation of tare
 * @return 4 -> 1 - weight present || 0 - no weight
 */
void loop() {
  // // For easier calibration
  // measure();
  // Serial.println(weight);
  // delay(2000);
  if (Serial.available()) {
    char command = Serial.read();

    if (command == '1') {
      bool itemRemoved = measure();
      if (count == 0) {
        weightThreshold = weight * epsilon;
      }

      if (!itemRemoved) {
        Serial.println(weight); // Send weight to Pi 5
      }
      else {
        errCount++;
        if (errCount = 2) {
          int removed = -1;
          Serial.println(removed); // signal; no weight on scale
        }
      }
      prevWeight = weight; // Could be used to check for weight drift
      count++;
    }

    else if (command == '2') {
      scale.set_offset(zeroFactor); // Auto-tare

      // Reset variables for new process
      count        = 0;
      errCount     = 0;
      weight       = 0.0f;
      prevWeight   = 0.0f;
      bool confirm = true;
      Serial.println(confirm); // Notify Pi 5 of tare completion
    }

    else if (command == '4') {
      bool isWeight = measure();
      Serial.println(isWeight); // signal 1 = weight present 0 = no weight
    }
  }
}

/**
 * This function is used to measure the weight on the scale.
 * Reads 20 values in and takes their average.
 * get_value() returns: average - OFFSET (zeroFactor)
 */
bool measure() {
  if (setupDone && scale.is_ready()) {
    scale.set_offset(zeroFactor); // Auto-tare
    delay(2);
    scale.set_scale(calibrationFactor); // Adjust to this calibration factor

    weight = scale.get_units(20);

    // Check if weight on scale was removed. Return true if less than 0.15lb (2.4oz)
    if (abs(weight) < 0.15f) {
      return true;
    }

    return false;
  }
}