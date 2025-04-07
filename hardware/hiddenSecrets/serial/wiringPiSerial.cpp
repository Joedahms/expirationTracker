#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringSerial.h>

int arduino_fd = -1;

int initSerialConnection(const char* device, int baudRate) {
  arduino_fd = serialOpen(device, baudRate);
  if (arduino_fd < 0) {
    fprintf(stderr, "Error: Unable to open serial device: %s\n", strerror(errno));
    return 1;
  }

  if (wiringPiSetup() == -1) {
    fprintf(stderr, "Error: Unable to initialize wiringPi\n");
    return 1;
  }

  serialFlush(arduino_fd);
  printf("Serial connection established on %s at %d baud.\n", device, baudRate);
  return 0;
}

// Send a single integer command (1 byte) and read the response
float sendCommandForFloat(int command) {
  serialFlush(arduino_fd);
  serialPutchar(arduino_fd, (unsigned char)command); // Send raw byte
  usleep(100000); // Wait 100ms for response

  unsigned char raw[4];
  int i = 0;

  int timeout = 100; // 100 x 1ms = 100ms max wait
  while (i < 4 && timeout--) {
    if (serialDataAvail(arduino_fd)) {
      raw[i++] = serialGetchar(arduino_fd);
    } else {
      usleep(1000); // Wait 1ms
    }
  }

  float result;
  memcpy(&result, raw, sizeof(float));
  return result;
}

int main(void) {
  const char* serialDevice = "/dev/ttyUSB0";
  int baud = 9600;

  if (initSerialConnection(serialDevice, baud) != 0)
    return 1;

  printf("Sending integer 1 to initialize Arduino...\n");
  serialPutchar(arduino_fd, 1); // Send setup command
  usleep(100000); // Optional wait

  sleep(1); // Delay before measurement

  printf("Sending integer 2 to request weight...\n");
  float weight = sendCommandForFloat(2);
  printf("Weight: %.2f\n", weight);

  serialClose(arduino_fd);
  return 0;
}
