cmake_minimum_required (VERSION 3.16)
project(expirationTracker DESCRIPTION "Track expiration dates" LANGUAGES CXX)

find_package (glog 0.4.0 REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

add_executable (expirationTracker 
  main.cpp
  
  # Display process after forked from main
  display/src/display_entry.h
  display/src/display_entry.cpp

  # Entry into SDL after forked from displayEntry
  display/src/sdl_entry.h
  display/src/sdl_entry.cpp

  display/src/button.h
  display/src/button.cpp
  display/src/game_global.h
  display/src/game.h
  display/src/game.cpp
  display/src/gameplay.h
  display/src/gameplay.cpp
  display/src/main_menu.h
  display/src/main_menu.cpp
  display/src/pause_menu.h
  display/src/pause_menu.cpp
  display/src/rectangle.h
  display/src/rectangle.cpp
  display/src/text.h
  display/src/text.cpp

  vision/src/vision_pipe.h
  vision/src/vision_pipe.cpp
)
target_link_libraries (expirationTracker glog::glog)
target_link_libraries(expirationTracker ${SDL2_LIBRARIES} SDL2_image)
target_link_libraries(expirationTracker SDL2_ttf)

