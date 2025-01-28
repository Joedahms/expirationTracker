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
  display/src/sdl_entry.h
  display/src/sdl_entry.cpp

  # GUI
  display/src/button.h
  display/src/button.cpp
  display/src/display_global.h
  display/src/display.h
  display/src/display.cpp
  display/src/rectangle.h
  display/src/rectangle.cpp
  display/src/text.h
  display/src/text.cpp

  # GUI States
  display/src/states/main_menu.h
  display/src/states/main_menu.cpp
  display/src/states/scanning.h
  display/src/states/scanning.cpp
  display/src/states/pause_menu.h
  display/src/states/pause_menu.cpp
  display/src/states/item_list.h
  display/src/states/item_list.cpp

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

