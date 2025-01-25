cmake_minimum_required(VERSION 3.16)
project(expirationTracker DESCRIPTION "Track expiration dates" LANGUAGES CXX)

# Find glog library
find_package(glog 0.4.0 REQUIRED)

# Add Darknet include and library paths
set(DARKNET_INCLUDE_DIR /home/loggs/Source/Repos/raspi-yolo/third_party/darknet/include) # Update with your actual path
set(DARKNET_LIB_DIR /home/loggs/Source/Repos/raspi-yolo/third_party/darknet)            # Path to the directory containing libdarknet.a

# Add executable
add_executable(expirationTracker 
  main.cpp
  display/src/display_pipe.cpp
  vision/src/vision_pipe.cpp
  vision/src/categorizeObjects.cpp
  hardware/src/hardware_pipe.cpp
)

# Add include path
target_include_directories(expirationTracker PRIVATE ${DARKNET_INCLUDE_DIR})

# Link Darknet and required libraries
target_link_libraries(expirationTracker 
  PRIVATE
  glog::glog
  ${DARKNET_LIB_DIR}/libdarknet.so # Link Darknet static library
  m
  pthread
)
