#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringSerial.h>

int arduino_fd = -1; // File descriptor for the serial port

// Initialize the serial connection to the Arduino
void initSerialConnection(const char* device, int baudRate) {
  arduino_fd = serialOpen(device, baudRate);
  if (arduino_fd < 0) {
    fprintf(stderr, "Error: Unable to open serial device %s\n", device);
    exit(1);
  }
  serialFlush(arduino_fd);
  printf("Serial connection established on %s at %d baud.\n", device, baudRate);
}

// Send a single command byte to the Arduino and read its response.
// 'command' is a value: 0=nothing, 1=get weight, 2=tare, 3=auto-calibrate.
// 'responseBuffer' should be a preallocated array to store the response.
// Returns the number of bytes received.
int sendCommand(unsigned char command,
                unsigned char* responseBuffer,
                int maxResponseLength) {
  // Send the command byte to the Arduino.
  serialPutchar(arduino_fd, command);
  // Optionally, wait a short time to allow the Arduino to process the command.
  usleep(10000); // 10 ms delay

  int bytesRead = 0;
  // Read available bytes (the response could be more than one byte)
  while (serialDataAvail(arduino_fd) > 0 && bytesRead < maxResponseLength) {
    int c = serialGetchar(arduino_fd);
    if (c < 0)
      break;
    responseBuffer[bytesRead++] = (unsigned char)c;
  }
  return bytesRead;
}

// Example usage
int main(void) {
  const char* serialDevice = "/dev/ttyUSB0";
  // Adjust as needed for your setup.
  int baud = 9600; // Use the baud rate set on your Arduino.
  initSerialConnection(serialDevice, baud);

  // Send a command to get weight (command = 1)
  unsigned char response[256] = {0};
  int responseLength          = sendCommand(1, response, sizeof(response));

  printf("Received %d bytes: ", responseLength);
  for (int i = 0; i < responseLength; i++) {
    printf("%02X ", response[i]);
  }
  printf("\n");
  serialClose(arduino_fd);
  return 0;
}