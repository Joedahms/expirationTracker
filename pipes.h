#ifndef PIPES_H
#define PIPES_H

// Define constants for readability
#define READ  0
#define WRITE 1

struct Pipes {
  int displayToHardware[2];
  int displayToVision[2];

  int hardwareToDisplay[2];
  int hardwareToVision[2];

  int visionToDisplay[2];
  int visionToHardware[2];
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
inline void initializePipes(Pipes& pipes) {
  pipe(pipes.displayToHardware);
  pipe(pipes.displayToVision);

  pipe(pipes.hardwareToDisplay);
  pipe(pipes.hardwareToVision);

  pipe(pipes.visionToDisplay);
  pipe(pipes.visionToHardware);
}

#endif
