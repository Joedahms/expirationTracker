#ifndef ENDPOINTS_H
#define ENDPOINTS_H

#include <string>

struct ExternalEndpoints {
  const std::string visionEndpoint   = "ipc:///tmp/vision_endpoint";
  const std::string hardwareEndpoint = "ipc:///tmp/hardware_endpoint";
  const std::string displayEndpoint  = "ipc:///tmp/display_endpoint";
};

struct Messages {
  // General indication that a message was received
  const std::string AFFIRMATIVE = "got it";

  // Indicate that the detection of a food item failed for a general reason
  const std::string ITEM_DETECTION_FAILED = "item detection failed";

  // Indiate that the detection of a food item succeeded
  const std::string ITEM_DETECTION_SUCCEEDED = "detection successful";

  // Send that message again. May not be ready to handle it yet.
  const std::string RETRANSMIT = "retransmit";
};

#endif
