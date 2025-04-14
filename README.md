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

# Setup
````plaintext
sudo chmod 777 install.sh
./install.sh \
````
### LAN Server Setup
````plaintext
FOR ETHERNET CONNECTION TO WORK:

Because there is no DHCP server inbetween the two devices in this ethernet connection,
we must set a static IP for both ethernet interfaces.

YOU CAN SET IT TO WHATEVER STATIC IPS YOU WANT. You just must update the config.json file in vision/ accordingly.
SERVER AND PI USE THIS, MAKE SURE BOTH HAVE ACCESS TO UPDATED VERSIONS.

Windows:
Open ncpa.cpl (Run > ncpa.cpl)

Right-click Ethernet > Properties

Select Internet Protocol Version 4 (TCP/IPv4) > click Properties

Set:

IP address: 192.168.137.1

Subnet mask: 255.255.255.0

Default gateway: leave blank

DNS server: leave blank

PI:

sudo nmcli connection modify "Wired connection 1"
ipv4.addresses 192.168.137.2/24
ipv4.dns 8.8.8.8
ipv4.method manual

sudo nmcli connection up "Wired connection 1"

REBOOT
````
### WiringPi Setup
#### For if you just need to make a file
````plaintext
gcc -o myapp myapp.cpp -l wiringPi
````
# Usage
````plaintext
source models-venv/bin/activate
$ ./expirationTracker
````
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
 
