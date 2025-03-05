# raspi-yolo
This repository is the software for our senior design project for Purdue Indianapolis course ECE492. We are aiming to engineer\
a device to reduce household food waste by tracking expiration dates.
\
\
The project consists of two teams, software and hardware. Hardware is responsible for the physical structure of the\
device as well as the electronic components. Software is responsible for all the software.\
We have split the software responsibilies up 3 ways, display, vision, and hardware. Display is responsible for creating\
a GUI for the touchscreen. The GUI is to allow the user to see what food they have, and when it expires. Vision is\
responsible for implementing the machine learning algorithms to identify when a food item expires. Hardware is responsible\
for controlling all hardware components such as the cameras, weight sensor, and platform motor.

# Team Members
- Caleb Rector (Hardware)
- John Teeguarden (Hardware)
- Geromy Cunningham (Software, Hardware)
- Logan Pelkey (Software, Vision)
- Joe Dahms (Software Display)

# Setup instructions

## Install necessary dependencies
#### Refresh local package information:
$ sudo apt update
#### Install build tools
$ sudo apt install make g++ cmake
#### Install Google logging library
$ sudo apt install libgoogle-glog-dev
#### Install SDL
$ sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev  
#### Install OPENCV
$ sudo apt install libopencv-dev
#### Install SQLite
$ sudo apt install sqlite3 libsqlite3-dev
#### Install mesa
$ sudo add-apt-repository ppa:kisak/kisak-mesa
#### Install zeroMQ
$ sudo apt-get install libzmq3-dev\
$ git clone https://github.com/zeromq/zmqpp.git \
$ cd zmqpp\
$ mkdir build && cd build\
$ cmake ..\
$ make\
$ sudo make install
#### Install Protobuf
$ sudo apt install protobuf-compiler libprotobuf-dev


## Setup Python virual environment
NOTE: To setup the Python virtual environment you must be in the build directory. Run all pip commands within the 
build directory.
#### Create the virtual environment
$ python3 -m venv models-venv
#### Activate it
$ source models-venv/bin/activate
#### Install EasyOCR
$ pip install easyocr
#### Install Efficientnett
$ pip install efficientnet
#### Install Tensorflow
$ pip install tensorflow
#### Install zeroMQ
$ pip install pyzmq

# Developer Notes
## Function Comment Format (javadoc)
/**  
 \* Detailed description of what function's purpose is  
 \*  
 \* Input:  
 \* @param Name of Input 1 Description of Input 1  
 \* @param Name of Input 2 Description of Input 2  
 \* @return Description of return value
 */  


