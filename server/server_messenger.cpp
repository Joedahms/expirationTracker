#include <filesystem>
#include <nlohmann/json.hpp>

#include "server_messenger.h"

ServerMessenger::ServerMessenger(zmqpp::context& context,
                                 const std::string& configFilename,
                                 Logger& logger)
    : replySocket(context, zmqpp::socket_type::reply) {
  std::filesystem::path currentPath = std::filesystem::current_path();
  std::filesystem::path filePath    = currentPath / configFilename;
  std::ifstream file(filePath);

  if (!file.is_open()) {
    std::cerr << "Error: Could not open the file." << std::endl;
  }

  nlohmann::json config = nlohmann::json::parse(file);

  std::string serverAddress = config["network"]["serverAddress"];

  try {
    this->replySocket.bind(serverAddress);
  } catch (const zmqpp::exception& e) {
    std::cerr << e.what();
    exit(1);
  }
}
