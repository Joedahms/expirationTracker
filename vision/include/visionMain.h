#ifndef VISION_PIPE_H
#define VISION_PIPE_H

#include <iostream>
#include <zmqpp/zmqpp.hpp>

#include "../../food_item.h"
#include "../../pipes.h"
#include "../include/ImageProcessor.h"
#include "../include/helperFunctions.h"

void visionEntry(struct Pipes,
                 zmqpp::context& context,
                 struct ExternalEndpoints externalEndpoints);
bool startPythonServer();
#endif
