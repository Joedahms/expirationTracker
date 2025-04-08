#ifndef CONFIG_H
#define CONFIG_H

struct Config {
  std::string ethernetIP;
  int serverPort;
  int heartbeatPort;
  int discoveryPort;
  bool useEthernet;
};

struct ServerAddress {
  std::string serverAddress;
  std::string heartbeatAddress;
};

#endif