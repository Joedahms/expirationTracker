#include <glog/logging.h>
#include <iostream>

#include "pipe.h"

// 0 reading
// 1 writing

void visionEntry(struct VisionPipes pipes) {
  LOG(INFO) << "Within vision process";
  // not ever used
  close(getPipe(pipes.fromDisplay, WRITE));
  close(getPipe(pipes.fromHardware, WRITE));
  close(getPipe(pipes.toDisplay, READ));
  close(getPipe(pipes.toHardware, READ));

  // not currently used
  close(getPipe(pipes.fromHardware, READ));
  close(getPipe(pipes.fromDisplay, READ));
  close(getPipe(pipes.toHardware, WRITE));

  LOG(INFO) << "Sending Message from Vision to Display";
  const char* message = "Hello from Vision!";
  write(getPipe(pipes.toDisplay, WRITE), message, strlen(message) + 1);
  // used
  close(getPipe(pipes.toDisplay, WRITE));
}