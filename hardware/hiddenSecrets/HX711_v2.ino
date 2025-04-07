
/*
 Example using the SparkFun HX711 breakout board with a scale
 By: Nathan Seidle
 SparkFun Electronics
 Date: November 19th, 2014
 License: This code is public domain but you buy me a beer if you use this and we meet
 someday (Beerware license).

 This is the calibration sketch. Use it to determine the calibration_factor that the main
 example uses. It also outputs the zero_factor useful for projects that have a permanent
 mass on the scale in between power cycles.

 Setup your scale and start the sketch WITHOUT a weight on the scale
 Once readings are displayed place the weight on the scale
 Press +/- or a/z to adjust the calibration_factor until the output readings match the
 known weight Use this calibration_factor on the example sketch

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

float calibration_factor =
    3300.00; // 400150.00 worked for the small setup I created. Will need to be redone for
             // the final "scale" created
float movingAverageSamples[MOV_AVG_SAMPLES];
float sum, movingAverage = 0, prevValue = 0.0f, prevSample;
float epsilon = 0.01;
int count     = 0;

float exponentialSumming(float data, bool reset);

void setup() {
  Serial.begin(9600);
  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press + or a to increase calibration factor");
  Serial.println("Press - or z to decrease calibration factor");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale();
  scale.tare(); // Reset the scale to 0
  scale.set_gain(64);

  long zero_factor = scale.read_average(); // Get a baseline reading
  Serial.print("Zero factor: "); // This can be used to remove the need to tare the scale.
                                 // Useful in permanent scale projects.
  Serial.println(zero_factor);

  for (int i = 0; i < MOV_AVG_SAMPLES; i++)
    movingAverageSamples[i] = 0; // initializing the array
}

void loop() {
  scale.set_scale(calibration_factor); // Adjust to this calibration factor

  for (int i = 0; i < MOV_AVG_SAMPLES - 1;
       i++) // Shifting samples the moving average is looking at
    movingAverageSamples[i] = movingAverageSamples[i + 1];

  movingAverageSamples[MOV_AVG_SAMPLES - 1] = scale.get_units(); // Gaining next sample
  sum                                       = 0;

  for (int i = 0; i < MOV_AVG_SAMPLES; i++) { // Determining the average value
    sum += movingAverageSamples[i];
    movingAverage = sum / MOV_AVG_SAMPLES;
  }

  // if (count % 5 == 0)
  // prevSample = abs(scale.get_units());

  if (count == 3) { // will check if weight is on scale every 20 iterations
    if (abs(exponentialSumming(movingAverage, 0)) >=
        abs(exponentialSumming(movingAverage, 0)) -
            (prevSample * epsilon)) { // weight on scale was removed
      // if (abs(exponentialSumming(movingAverage, 0)) - prevSample >=  epsilon) {
      Serial.print("\nTaring scale...\n");
      scale.tare();
    }
    else if (exponentialSumming(movingAverage, 0) <= 0) {
      Serial.print("\nTaring scale...\n");
      scale.tare();
    }
    else
      count = 0;
  }
  else
    prevSample = abs(exponentialSumming(movingAverage, 0));

  count++;

  Serial.print("Reading: ");
  Serial.print(exponentialSumming(movingAverage, 0), 4);
  Serial.print(
      " kg (moving exp average)"); // Change this to kg and re-adjust the calibration
                                   // factor if you follow SI units like a sane person
  // Serial.print(" calibration_factor: ");
  // Serial.print(calibration_factor);
  // Serial.print(", ");
  // Serial.print(movingAverage, 4);
  // Serial.print(" kg (moving average)");
  Serial.println();

  if (Serial.available()) {
    char temp = Serial.read();
    if (temp == '+' || temp == 'a')
      calibration_factor += 10;
    else if (temp == '-' || temp == 'z')
      calibration_factor -= 100;
    else if (temp == 'r') {
      scale.tare();
      exponentialSumming(0, 1);
    }
  }

  // delay(1000);
}

float exponentialSumming(float data,
                         bool reset) { // implementing a moving exponential average filter

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
