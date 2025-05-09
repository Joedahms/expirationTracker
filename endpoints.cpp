#include "endpoints.h"

const std::string ExternalEndpoints::hardwareEndpoint = "ipc:///tmp/hardware_endpoint";
const std::string ExternalEndpoints::displayEndpoint  = "ipc:///tmp/display_endpoint";

const std::string Messages::AFFIRMATIVE              = "affirmative";
const std::string Messages::ITEM_DETECTION_FAILED    = "item detection failed";
const std::string Messages::ITEM_DETECTION_SUCCEEDED = "detection successful";
const std::string Messages::RETRANSMIT               = "retransmit";
const std::string Messages::START_SCAN               = "start scan";
const std::string Messages::ZERO_WEIGHT              = "zero weight";

const std::string Messages::RETRY    = "retry";
const std::string Messages::OVERRIDE = "override";
const std::string Messages::CANCEL   = "cancel";

const std::string Messages::SCAN_CANCELLED = "scan cancelled";
