cmake_minimum_required(VERSION 3.16)
project(expirationTracker DESCRIPTION "Track expiration dates" LANGUAGES CXX)

# Find glog library
find_package(glog 0.4.0 REQUIRED)

# Find Darknet include directory
set(DARKNET_INCLUDE_DIR ~/Source/Repos/raspi-yolo/yoloDarknet/include)
set(DARKNET_LIBRARY ~/Source/Repos/raspi-yolo/yoloDarknet/libdarknet.so) # Update with your actual path

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
  ${DARKNET_LIBRARY} # Link Darknet library dynamically
)
