#ifndef HARDWARE_ENTRY_H
#define HARDWARE_ENTRY_H

#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <string>

#include "../../endpoints.h"
#include "../../food_item.h"
#include "../../logger.h"
#include "./hardware.h"

void hardwareEntry(zmqpp::context& context);

#endif
