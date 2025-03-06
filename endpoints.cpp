#include "endpoints.h"

const std::string ExternalEndpoints::visionEndpoint   = "ipc:///tmp/vision_endpoint";
const std::string ExternalEndpoints::hardwareEndpoint = "ipc:///tmp/hardware_endpoint";
const std::string ExternalEndpoints::displayEndpoint  = "ipc:///tmp/display_endpoint";

const std::string Messages::AFFIRMATIVE              = "affirmative";
const std::string Messages::ITEM_DETECTION_FAILED    = "item detection failed";
const std::string Messages::ITEM_DETECTION_SUCCEEDED = "detection successful";
const std::string Messages::RETRANSMIT               = "retransmit";
const std::string Messages::START_SCAN               = "start scan";
