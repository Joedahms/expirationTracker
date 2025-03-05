#ifndef ENDPOINTS_H
#define ENDPOINTS_H

#include <string>

struct ExternalEndpoints {
  const std::string visionEndpoint   = "ipc:///tmp/vision_endpoint";
  const std::string hardwareEndpoint = "ipc:///tmp/hardware_endpoint";
  const std::string displayEndpoint  = "ipc:///tmp/display_endpoint";
};

struct Messages {
  const std::string ITEM_DETECTION_FAILED = "item detection failed";
};

#endif
