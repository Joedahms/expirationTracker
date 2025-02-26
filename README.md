# raspi-yolo

## Function Comment Format
/**  
 \* Detailed description of what function's purpose is  
 \*  
 \* Input:  
 \* @param Name of Input 1 Description of Input 1  
 \* @param Name of Input 2 Description of Input 2  
 \* @return Description of return value
 */  

# Create a build and a YOLO directory within the root of the repo

Navigate into YOLO and run:
git clone https://github.com/lpelkey23/yolov4-tiny

cd into the directory it creates (yolov4-tiny)

run make

grab the weights file for tinyv4:
wget https://github.com/AlexeyAB/darknet/releases/download/yolov4/yolov4-tiny.weights

cd ../../build

run cmake ..
run make

# Had to install to run repo:
sudo apt update
sudo apt upgrade
sudo apt install make
sudo apt install g++
sudo apt install cmake
sudo apt install libgoogle-glog-dev
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev  
sudo apt install libopencv-dev
sudo apt install sqlite3
sudo apt install libsqlite3-dev
sudo add-apt-repository ppa:kisak/kisak-mesa
sudo apt update
sudo apt upgrade

# Using Models
# BE IN BUILD DIRECTORY!!!!!!
### Create the virtual environment
python3 -m venv models-venv

### Activate it
source models-venv/bin/activate

### Install EasyOCR inside the virtual environment
pip install easyocr

### Install Efficientnet within the virtual environment
pip install efficientnet

### Install Tensorflow
pip install tensorflow

