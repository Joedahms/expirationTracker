#ifndef VISION_PIPE_H
#define VISION_PIPE_H

#include <iostream>
#include <zmqpp/zmqpp.hpp>

#include "../../endpoints.h"
#include "../../food_item.h"
#include "../../logger.h"
#include "../include/ImageProcessor.h"
#include "../include/helperFunctions.h"

void visionEntry(zmqpp::context& context, struct ExternalEndpoints externalEndpoints);
bool startPythonServer(Logger& logger);

#endif
