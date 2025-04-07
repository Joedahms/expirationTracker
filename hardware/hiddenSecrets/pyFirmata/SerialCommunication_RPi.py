#Serial communication between the Pi and Arduino
#baudrates of the Pi and the Arduino must match each other
#only requires a single USB connection between the Pi and the Arduino


import serial
import time

#Configure serial port
arduino = serial.Serial(port = "/dev/ttyACM0", baudrate = 9600, timeout = 0.1)  
              #ACM0 will be called something else depending on which USB port your Arduino is connected into
              #Can find Arduino serial port on Pi by running "ls dev/tty*" in the terminal

data = 0
def write_read(x):    #input and output are strings
  arduino.write(bytes(x, 'utf-8'))    #writing to the Arduino
  time.sleep(0.05)
  data = arduino.readline().decode('utf-8').rstrip()    #reading from the Arduino
  
  return data

for i in range(0,5):    #can be replaced with an infinite loop, if needed
  print(write_read('4'))
  print("Waiting 2 seconds\n")
  time.sleep(2)

  print(write_read('8'))
  print("Waiting 2 seconds\n")
  time.sleep(2)

print("Loop terminated")

arduino.close()
