#ifndef PIPES_H
#define PIPES_H

// Define constants for readability
#define READ  0
#define WRITE 1
#define EMPTY "EMPTY" // Sent when the queue is empty
#define FULL  "FULL"  // Sent when the queue is full
#define READY "READY" // Sent when the queue has space or data
#define DONE  "DONE"

// Struct for Display process pipes
struct DisplayPipes {
  int toHardware[2];   // Display -> Hardware
  int fromHardware[2]; // Hardware -> Display

  int toVision[2];   // Display -> Vision
  int fromVision[2]; // Vision -> Display
};

// Struct for Vision process pipes
struct VisionPipes {
  int toDisplay[2];   // Vision -> Display
  int fromDisplay[2]; // Display -> Vision

  int toHardware[2];   // Vision -> Hardware
  int fromHardware[2]; // Hardware -> Vision
};

// Struct for Hardware process pipes
struct HardwarePipes {
  int toDisplay[2];   // Hardware -> Display
  int fromDisplay[2]; // Display -> Hardware

  int toVision[2];   // Hardware -> Vision
  int fromVision[2]; // Vision -> Hardware
};

/**
 * Ensure that the read ends in one process are equivalent to the write ends in the
 * others.
 *
 * Input:
 * - Pipes for the main display process
 * - Pipes for the main vision process
 * - Pipes for the main hardware process
 * Output: None
 */
inline void initializePipes(DisplayPipes& display,
                            VisionPipes& vision,
                            HardwarePipes& hardware) {
  // Display ↔ Hardware
  pipe(display.toHardware);
  hardware.fromDisplay[READ]  = display.toHardware[READ];
  hardware.fromDisplay[WRITE] = display.toHardware[WRITE];

  pipe(display.fromHardware);
  hardware.toDisplay[READ]  = display.fromHardware[READ];
  hardware.toDisplay[WRITE] = display.fromHardware[WRITE];

  // Display ↔ Vision
  pipe(display.toVision);
  vision.fromDisplay[READ]  = display.toVision[READ];
  vision.fromDisplay[WRITE] = display.toVision[WRITE];

  pipe(display.fromVision);
  vision.toDisplay[READ]  = display.fromVision[READ];
  vision.toDisplay[WRITE] = display.fromVision[WRITE];

  // Vision ↔ Hardware
  pipe(vision.toHardware);
  hardware.fromVision[READ]  = vision.toHardware[READ];
  hardware.fromVision[WRITE] = vision.toHardware[WRITE];

  pipe(vision.fromHardware);
  hardware.toVision[READ]  = vision.fromHardware[READ];
  hardware.toVision[WRITE] = vision.fromHardware[WRITE];
}

#endif
