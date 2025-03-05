#ifndef HARDWARE_ENTRY_H
#define HARDWARE_ENTRY_H

#include "../../pipes.h"
#include <string>

void hardwareEntry(zmqpp::context& context, struct ExternalEndpoints externalEndpoints);
void redoThis(struct Pipes pipes);
void sendImagesWithinDirectory(int, const std::string&);

#endif
