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

  # Handler for messages from other processes and the display engine
  display/src/display_handler.cpp

  # Database operations on food items
  display/src/sql_food.cpp

  # Entry into engine after forked from displayEntry
  display/src/engine/engine_entry.cpp

  # GUI 
  display/src/engine/display_engine.cpp
  display/src/engine/sdl_debug.cpp

  # GUI Elements
  display/src/engine/elements/element.cpp
  display/src/engine/elements/button.cpp
  display/src/engine/elements/text.cpp
  display/src/engine/elements/panel.cpp
  display/src/engine/elements/scroll_box.cpp
  display/src/engine/elements/number_setting.cpp

  # GUI States
  display/src/engine/states/state.cpp
  display/src/engine/states/main_menu.cpp
  display/src/engine/states/scanning.cpp
  display/src/engine/states/pause_menu.cpp
  display/src/engine/states/item_list.cpp

  # Vision process after forked from main
  vision/src/visionMain.cpp
  vision/src/helperFunctions.cpp
  vision/src/ImageProcessor.cpp
  vision/src/ModelHandler.cpp
  vision/src/ObjectClassifier.cpp
  vision/src/TextClassifier.cpp



  #Hardware process after forked from main
  hardware/src/hardware_entry.cpp
  hardware/src/io.cpp
)

target_include_directories(expirationTracker PRIVATE ${OpenCV_INCLUDE_DIRS})

target_link_libraries(expirationTracker
  glog::glog
  ${SDL2_LIBRARIES} SDL2_image
  SDL2_ttf
  ${OpenCV_LIBS}
  sqlite3
)
