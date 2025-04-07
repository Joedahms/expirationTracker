#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

// This function sets up the serial communication.
// device: The serial port (e.g., "/dev/ttyACM0")
// baudRate: The baud rate (e.g., B9600)
int initSerialCommunication(const char* device, speed_t baudRate) {
  // Open the serial port for reading and writing. "/dev/ttyACM0"
  int fd = open(device, O_RDWR | O_NOCTTY | O_SYNC);
  if (fd < 0) {
    perror("open");
    return -1;
  }
  // Get current terminal attributes.
  struct termios tty;
  if (tcgetattr(fd, &tty) != 0) {
    perror("tcgetattr");
    close(fd);
    return -1;
  }

  // Set input and output baud rate.
  cfsetospeed(&tty, baudRate);
  cfsetispeed(&tty, baudRate);

  // Configure the terminal in raw mode.
  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit characters
  tty.c_iflag &= ~IGNBRK;                     // disable break processing
  tty.c_lflag     = 0; // no signaling chars, no echo, no canonical processing
  tty.c_oflag     = 0; // no remapping, no delays
  tty.c_cc[VMIN]  = 0; // non-blocking read
  tty.c_cc[VTIME] = 5; // 0.5 seconds read timeout

  // Shut off xon/xoff control.
  tty.c_iflag &= ~(IXON | IXOFF | IXANY);
  tty.c_cflag |= (CLOCAL | CREAD);   // enable receiver, ignore modem control lines
  tty.c_cflag &= ~(PARENB | PARODD); // shut off parity
  tty.c_cflag &= ~CSTOPB;            // one stop bit
  tty.c_cflag &= ~CRTSCTS;           // no hardware flow control

  // Set attributes immediately.
  if (tcsetattr(fd, TCSANOW, &tty) != 0) {
    perror("tcsetattr");
    close(fd);
    return -1;
  }

  return fd;
}

// This function sends a single command byte to the Arduino.
bool sendCommandByte(int fd, uint8_t command) {
  ssize_t n = write(fd, &command, 1);
  return (n == 1);
}

// // Example usage:
// int test0() {
//   // Initialize serial connection (using "/dev/ttyACM0" and 9600 baud)
//   int fd = initSerialCommunication("/dev/ttyACM0", B9600);
//   if (fd < 0) {
//     return -1;
//   }
//   // Send a command byte (for example, 0xA5)
//   if (sendCommandByte(fd, 0xA5)) {
//     printf("Command sent successfully\n");
//   }
//   else {
//     printf("Failed to send command\n");
//   }
//   close(fd);
//   return 0;
// }

// This function attempts to read up to 'bufferSize' bytes into 'buffer'.
// It returns the number of bytes read, or -1 if an error occurs.
ssize_t readData(int fd, uint8_t* buffer, size_t bufferSize) {
  ssize_t bytesRead = read(fd, buffer, bufferSize);
  if (bytesRead < 0) {
    perror("read");
  }
  return bytesRead;
}

// // Example usage:
// int test1() {
//   // Initialize serial connection (using "/dev/ttyACM0" and 9600 baud)
//   int fd = initSerialCommunication("/dev/ttyACM0", B9600);
//   if (fd < 0) {
//     return -1;
//   }

//   // Send a command byte (for example, 0xA5)
//   if (sendCommandByte(fd, 0xA5)) {
//     printf("Command sent successfully\n");
//   }
//   else {
//     printf("Failed to send command\n");
//   }

//   // Read a response from the Arduino.
//   // For example, we try to read up to 10 bytes.
//   uint8_t buffer[10];
//   ssize_t n = readData(fd, buffer, sizeof(buffer));

//   if (n > 0) {
//     printf("Received %zd bytes: ", n);
//     for (ssize_t i = 0; i < n; i++) {
//       printf("0x%02X ", buffer[i]);
//     }
//     printf("\n");
//   }
//   else if (n == 0) {
//     printf("No data received (timeout)\n");
//   }
//   close(fd);
//   return 0;
// }