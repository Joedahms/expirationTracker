#ifndef LOGGER_H
#define LOGGER_H

#include <ctime>
#include <fstream>
#include <iostream>
#include <string>

class Logger {
private:
  std::ofstream logFile; // The file stream as a class member
  std::string filename;  // Store the filename for potential reopening

public:
  // Constructor opens the file
  Logger(const std::string& logFilename) : filename(logFilename) {
    // Open file in append mode to preserve previous log entries
    logFile.open(filename, std::ios::app);

    if (!logFile.is_open()) {
      std::cerr << "Failed to open log file: " << filename << std::endl;
    }
  }

  // Destructor ensures the file is closed when the logger is destroyed
  ~Logger() {
    if (logFile.is_open()) {
      logFile.close();
    }
  }

  // Log a message with timestamp
  void log(const std::string& message) {
    if (logFile.is_open()) {
      // Get current time
      time_t now = time(nullptr);
      char timeStr[26];
      ctime_r(&now, timeStr);

      // Remove newline character from timeStr
      timeStr[24] = '\0';

      // Write timestamped message to log
      logFile << "[" << timeStr << "] " << message << std::endl;

      // Flush to ensure the message is written even if the program crashes
      logFile.flush();
    }
  }

  // Check if the log file is open
  bool isOpen() const { return logFile.is_open(); }

  // Manually close the file if needed
  void close() {
    if (logFile.is_open()) {
      logFile.close();
    }
  }

  // Reopen the file if needed (e.g., after log rotation)
  bool reopen() {
    if (logFile.is_open()) {
      logFile.close();
    }

    logFile.open(filename, std::ios::app);
    return logFile.is_open();
  }
};

#endif
