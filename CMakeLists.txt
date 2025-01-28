cmake_minimum_required(VERSION 3.16)
project(expirationTracker DESCRIPTION "Track expiration dates" LANGUAGES CXX)

find_package (glog 0.4.0 REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

# Find Darknet include directory
set(DARKNET_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/yoloDarknet/include)
set(DARKNET_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/yoloDarknet/libdarknet.so) # Update with your actual path

# Add executable
add_executable(expirationTracker 
main.cpp

# Display process after forked from main
display/src/display_entry.h
display/src/display_entry.cpp

# Entry into SDL after forked from displayEntry
display/src/sdl_entry.cpp
display/src/button.cpp
display/src/display.cpp
display/src/gameplay.cpp
display/src/main_menu.cpp
display/src/pause_menu.cpp
display/src/rectangle.cpp
display/src/text.cpp

# Vision process after forked from main
vision/src/vision_pipe.cpp
vision/src/categorizeObjects.cpp

#Hardware process after forked from main
hardware/src/hardware_pipe.cpp
)

target_include_directories(expirationTracker PRIVATE ${DARKNET_INCLUDE_DIR})

target_link_libraries(expirationTracker 
  PRIVATE
  glog::glog
  ${DARKNET_LIBRARY} # Link Darknet library dynamically
  ${SDL2_LIBRARIES} SDL2_image
  SDL2_ttf
)

