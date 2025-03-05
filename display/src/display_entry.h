#ifndef DISPLAY_PIPE_H
#define DISPLAY_PIPE_H

#include <zmqpp/zmqpp.hpp>

#include "../../pipes.h"

void displayEntry(zmqpp::context& context, struct ExternalEndpoints externalEndpoints);

#endif
