#ifndef VISION_ENDPOINTS_H
#define VISION_ENDPOINTS_H

#include <string>

struct VisionExternalEndpoints {
  inline static const std::string textClassifierEndpoint =
      "ipc:///tmp/text_classifier_endpoint";
  inline static const std::string pythonServerEndpoint =
      "ipc:///tmp/python_server_endpoint";
};

#endif