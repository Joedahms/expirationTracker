#ifndef VISION_PIPE_H
#define VISION_PIPE_H

#include <iostream>
#include <zmqpp/zmqpp.hpp>

#include "../../endpoints.h"
#include "../../food_item.h"
#include "../../logger.h"
#include "../include/ImageProcessor.h"
#include "../include/helperFunctions.h"

void visionEntry(zmqpp::context& context);
constexpr int MAX_SERVER_RETRIES = 5;

void visionEntry(zmqpp::context&, const ExternalEndpoints&);
bool startPythonServer(const Logger&);
bool startSignalCheck(zmqpp::socket&, const Logger&, FoodItem&, zmqpp::poller&);
#endif
