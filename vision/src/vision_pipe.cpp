#include <glog/logging.h>
#include <iostream>

#include "vision_pipe.h"

// 0 reading
// 1 writing

void visionEntry(struct VisionPipes pipes) {
  LOG(INFO) << "Within vision process";

  // not ever used
  close(pipes.fromDisplay[WRITE]);
  close(pipes.fromHardware[WRITE]);
  close(pipes.toDisplay[READ]);
  close(pipes.toHardware[READ]);

  // not currently used
  close(pipes.fromHardware[READ]);
  close(pipes.fromDisplay[READ]);
  close(pipes.toHardware[WRITE]);

  // used
  close(pipes.toDisplay[WRITE]);

  LOG(INFO) << "Sending Message from Vision to Display";
  const char* message = "Hello from Vision!";
  write(pipes.toDisplay[WRITE], message, strlen(message) + 1);
}
