#ifndef CONTROLS_H
#define CONTROLS_H

#include <string>

void setup();
void rotateMotor();
void takePhotos(int angle);
void takePic(char* filename);
void sendDataToVision(const std::string IMAGE_DIR, float weight);
void rotateAndCapture();

#endif