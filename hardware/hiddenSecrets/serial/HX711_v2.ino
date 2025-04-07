
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

#define LOADCELL_DOUT_PIN 3  // 6
#define LOADCELL_SCK_PIN  2  // 7
#define MOV_AVG_SAMPLES   30 // Number of samples read by the moving average algorithm

HX711 scale;

// 400150.00 worked for the small setup I created.
// Will need to be redone for the final "scale" created
float calibrationFactor = 3300.00;
float movingAverage     = 0.0f;
float prevValue         = 0.0f;
float epsilon           = 0.01f;
float sum, prevSample;
long zero_factor, weight_threshold;
bool setupDone = false;
int count      = 0;

float movingAverageSamples[MOV_AVG_SAMPLES];
float exponentialSumming(float data, bool reset);

void setup() {
  Serial.begin(9600);
  // wait for serial port to connect. Needed for native USB port only
  while (!Serial) {
  };
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale();
  scale.tare(); // Reset the scale to 0
  scale.set_gain(64);

  // This can be used to remove the need to tare the scale.
  // Useful in permanent scale projects.
  zero_factor = scale.read_average(); // Get a baseline reading

  for (int i = 0; i < MOV_AVG_SAMPLES; i++) {
    movingAverageSamples[i] = 0; // initializing the array
  }

  Serial.println(zero_factor);
  setupDone = true;
}

void loop() {
  if (Serial.available()) {
    int command - Serial.read();
    if (command == 2) {
      bool itemRemoved = measure();
      if (count == 0) {
        weight_threshold =
            (abs(exponentialSumming(movingAverage, 0)) - zero_factor) * .10;
      }
      count++;
      if (!itemRemoved) {
        Serial.print(exponentialSumming(movingAverage, 0), 4);
      }
      else {
        Serial.print(-1);
      }
    }
    else if (command == 4) {
      scale.tare();
      long item_factor = scale.read_average();
      if (abs(item_factor - zero_factor) > weight_threshold) {
        scale.set_scale(calibrationFactor);
      }
    }
    else if (command == 1) {
      setup();
    }
  }
}

// This function is used to measure the weight on the scale
bool measure() {
  if (setupDone && scale.is_ready()) {
    scale.set_scale(calibrationFactor); // Adjust to this calibration factor

    // Shifting samples the moving average is looking at
    for (int i = 0; i < MOV_AVG_SAMPLES - 1; i++) {
      movingAverageSamples[i] = movingAverageSamples[i + 1];
    }
    // Gaining next sample
    movingAverageSamples[MOV_AVG_SAMPLES - 1] = scale.get_units();
    sum                                       = 0;
    // Determining the average value
    for (int i = 0; i < MOV_AVG_SAMPLES; i++) {
      sum += movingAverageSamples[i];
      movingAverage = sum / MOV_AVG_SAMPLES;
    }

    // weight on scale was removed
    if (abs(exponentialSumming(movingAverage, 0)) >=
        (abs(exponentialSumming(movingAverage, 0)) - prevSample) * epsilon) {
      return true;
    }
    else {
      prevSample = abs(exponentialSumming(movingAverage, 0));
    }
    return false;
  }
}

// implementing a moving exponential average filter
float exponentialSumming(float data, bool reset) {
  float smoothingFactor, currentValue;
  smoothingFactor = 0.4;

  if (reset) {
    prevValue = 0;
    return 0;
  }

  currentValue = prevValue + smoothingFactor * (data - prevValue);
  prevValue    = currentValue;

  return currentValue;
}