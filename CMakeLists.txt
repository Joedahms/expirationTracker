cmake_minimum_required (VERSION 3.16)
project(expirationTracker DESCRIPTION "Track expiration dates" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(CXXFLAGS  "-Wall")
set(CMAKE_CXX_FLAGS "${CXXFLAGS}")

find_package (glog 0.4.0 REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(OpenCV REQUIRED)
find_package(SQLite3 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

add_executable (expirationTracker 
  main.cpp
  food_item.cpp
  
  # Display process after forked from main
  display/src/display_entry.cpp

  # Handlers for messages from sdl and external sources
  display/src/external_handler.cpp
  display/src/sdl_handler.cpp

  # Entry into SDL after forked from displayEntry
  display/src/sdl2/sdl_entry.cpp

  # GUI
  display/src/sdl2/button.cpp
  display/src/sdl2/display.cpp
  display/src/sdl2/rectangle.cpp
  display/src/sdl2/text.cpp

  # GUI States
  display/src/sdl2/states/main_menu.cpp
  display/src/sdl2/states/scanning.cpp
  display/src/sdl2/states/pause_menu.cpp
  display/src/sdl2/states/item_list.cpp

  # Vision process after forked from main
  vision/src/vision_pipe.cpp
  vision/src/categorizeObjects.cpp

  #Hardware process after forked from main
  hardware/src/hardware_pipe.cpp
)

target_include_directories(expirationTracker PRIVATE ${OpenCV_INCLUDE_DIRS})

target_link_libraries(expirationTracker
  glog::glog
  ${SDL2_LIBRARIES} SDL2_image
  SDL2_ttf
  ${OpenCV_LIBS}
  sqlite3
)
