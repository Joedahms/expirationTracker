#ifndef PIPES_H
#define PIPES_H

// Define constants for readability
constexpr int READ = 0;  // Read end of the pipe
constexpr int WRITE = 1; // Write end of the pipe

// Struct for Display process pipes
struct DisplayPipes {
  int toHardware[2];  // Display -> Hardware
  int fromHardware[2]; // Hardware -> Display

  int toVision[2];    // Display -> Vision
  int fromVision[2];  // Vision -> Display
};

// Struct for Vision process pipes
struct VisionPipes {
  int toDisplay[2];   // Vision -> Display
  int fromDisplay[2]; // Display -> Vision

  int toHardware[2];  // Vision -> Hardware
  int fromHardware[2]; // Hardware -> Vision
};

// Struct for Hardware process pipes
struct HardwarePipes {
  int toDisplay[2];   // Hardware -> Display
  int fromDisplay[2]; // Display -> Hardware

  int toVision[2];    // Hardware -> Vision
  int fromVision[2];  // Vision -> Hardware
};

inline int getPipe(int pipes[2], int mode) {
  return pipes[mode];
}

#endif