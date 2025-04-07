/*
 Example using the SparkFun HX711 breakout board with a scale
 By: Nathan Seidle
 SparkFun Electronics
 Date: November 19th, 2014
 License: This code is public domain but you buy me a beer if you use this and we meet
 someday (Beerware license).

 This is the calibration sketch. Use it to determine the calibrationFactor that the main
 example uses. It also outputs the zero_factor useful for projects that have a permanent
 mass on the scale in between power cycles.

 Setup your scale and start the sketch WITHOUT a weight on the scale
 Once readings are displayed place the weight on the scale
 Press +/- or a/z to adjust the calibrationFactor until the output readings match the
 known weight Use this calibrationFactor on the example sketch

 This example assumes pounds (lbs). If you prefer kilograms, change the Serial.print("
 lbs"); line to kg. The calibration factor will be significantly different but it will be
 linearly related to lbs (1 lbs = 0.453592 kg).

 Your calibration factor may be very positive or very negative. It all depends on the
 setup of your scale system and the direction the sensors deflect from zero state This
 example code uses bogde's excellent library:"https://github.com/bogde/HX711" bogde's
 library is released under a GNU GENERAL PUBLIC LICENSE Arduino pin 2 -> HX711 CLK 3 ->
 DOUT 5V -> VCC GND -> GND

 Most any pin on the Arduino Uno will be compatible with DOUT/CLK.

 The HX711 board can be powered from 2.7V to 5V so the Arduino 5V power should be fine.
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
  setupDone = true;
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
 * @return 4 -> 1 for no weight || 0 for weight
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
      bool noWeight = measure();
      Serial.println(noWeight); // signal 1 = no weight 0 = weight
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