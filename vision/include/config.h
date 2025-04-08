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

struct NotifyServerConstants {
  inline static const std::string start     = "start";
  inline static const std::string stop      = "stop";
  inline static const std::string connected = "connected";
};

#endif