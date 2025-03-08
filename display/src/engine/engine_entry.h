#ifndef ENGINE_ENTRY_H
#define ENGINE_ENTRY_H

#include <zmqpp/zmqpp.hpp>

void engineEntry(const zmqpp::context& context, const std::string& engineEndpoint);

#endif
