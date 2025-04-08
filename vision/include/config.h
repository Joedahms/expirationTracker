#ifndef CONFIG_H
#define CONFIG_H

struct Config {
  int serverPort;
  int heartbeatPort;
  bool useEthernet;
};

struct ServerAddress {
  std::string serverAddress;
  std::string heartbeatAddress;
};

#endif