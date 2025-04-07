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

int readLineFromArduino(char* buffer, int maxLen) {
  int i       = 0;
  int timeout = 1000; // 1000 ms timeout
  while (i < maxLen - 1 && timeout--) {
    if (serialDataAvail(arduino_fd)) {
      char c = serialGetchar(arduino_fd);
      if (c == '\n')
        break;
      buffer[i++] = c;
    }
    else {
      usleep(1000); // 1 ms wait
    }
  }
  buffer[i] = '\0';
  return i;
}

int main(void) {
  const char* serialDevice = "/dev/ttyACM0";
  int baud                 = 9600;

  if (initSerialConnection(serialDevice, baud) != 0)
    return 1;

  char input[10];
  char response[64];

  printf("Command Menu:\n");
  printf(" 1 = get weight\n 2 = tare/end\n 4 = confirm presence\n q = quit\n");

  while (1) {
    printf("Enter command: ");
    fflush(stdout);
    if (!fgets(input, sizeof(input), stdin)) {
      break;
    }
    input[strcspn(input, "\n")] = 0;

    if (strcmp(input, "q") == 0 || strcmp(input, "Q") == 0) {
      printf("Exiting...\n");
      break;
    }

    if (strcmp(input, "1") == 0) {
      printf("Sending command '1' (get weight)...\n");
      serialPutchar(arduino_fd, '1');
      readLineFromArduino(response, sizeof(response));
      float weight = atof(response);
      printf("Received weight: %.2f\n", weight);
    }
    else if (strcmp(input, "2") == 0) {
      printf("Sending command '2' (tare/end)...\n");
      serialPutchar(arduino_fd, '2');
      readLineFromArduino(response, sizeof(response));
      bool success = atoi(response);
      printf("Tare result: %s\n", success ? "OK" : "Failed");
    }
    else if (strcmp(input, "4") == 0) {
      printf("Sending command '4' (confirm presence)...\n");
      serialPutchar(arduino_fd, '4');
      readLineFromArduino(response, sizeof(response));
      bool present = atoi(response);
      printf("Weight detected: %s\n", present ? "YES" : "NO");
    }
    else {
      printf("Unknown command. Try 1, 2, 4, or q.\n");
    }
  }

  serialClose(arduino_fd);
  return 0;
}
