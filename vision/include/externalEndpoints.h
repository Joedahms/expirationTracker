#ifndef VISION_ENDPOINTS_H
#define VISION_ENDPOINTS_H

#include <string>

struct VisionExternalEndpoints {
  inline static const std::string textClassifierEndpoint =
      "ipc:///tmp/text_classifier_endpoint";
  inline static const std::string listenerEndpoint   = "ipc:///tmp/listener_endpoint";
  inline static const std::string visionMainEndpoint = "ipc:///tmp/visionMain_endpoint";
};

#endif