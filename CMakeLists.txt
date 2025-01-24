cmake_minimum_required (VERSION 3.16)
project(expirationTracker DESCRIPTION "Track expiration dates" LANGUAGES CXX)

find_package (glog 0.4.0 REQUIRED)

add_executable (expirationTracker 
  main.cpp
  display/src/pipe.cpp
  vision/src/pipe.cpp
)
target_link_libraries (expirationTracker glog::glog)


