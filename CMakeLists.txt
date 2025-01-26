cmake_minimum_required (VERSION 3.16)
project(expirationTracker DESCRIPTION "Track expiration dates" LANGUAGES CXX)

find_package (glog 0.4.0 REQUIRED)

add_executable (expirationTracker 
  main.cpp
  
  # Display process after forked from main
  display/src/display_entry.h
  display/src/display_entry.cpp

  # Entry into SDL after forked from displayEntry
  display/src/sdl_entry.h
  display/src/sdl_entry.cpp

  vision/src/vision_pipe.h
  vision/src/vision_pipe.cpp
)
target_link_libraries (expirationTracker glog::glog)


