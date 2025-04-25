#pySerial library will need to be installed
#https://pypi.org/project/pyserial/

import serial       
import datetime

#Configure serial port
ser = serial.Serial("/dev/ttyACM0", 9600)   #9600 is the baud rate used in the arduino
                                            #ttyACM0 is the serial port that the Arduino is connected to, 
                                            #name may change depending on which USB port it's connected to 
#Open log file
log_file = open("sensor_data.log", "a")

while True:
    data_line = ser.readline().decode('utf-8').rstrip() #Reading data from serial
    timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    log_file.write(f"{timestamp}, {data_line}\n")       #Write data to log file

log_file.close()
ser.close()