cmake_minimum_required(VERSION 3.16)
project(expirationTracker DESCRIPTION "Track expiration dates" LANGUAGES CXX)

# Set build type and C++ standard
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags (only compiler flags, not linker flags)
set(CXXFLAGS "-Wall")
set(CMAKE_CXX_FLAGS "${CXXFLAGS}")

# Find required packages
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(OpenCV REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Protobuf REQUIRED)

pkg_check_modules(ZMQ REQUIRED libzmq)

set(ZMQPP_INCLUDE_DIRS "/usr/local/include")
set(ZMQPP_LIBRARIES "zmqpp")

# Include directories for all libraries
include_directories(
  ${SDL2_INCLUDE_DIRS} 
  ${SDL2_IMAGE_INCLUDE_DIRS}
  ${ZMQ_INCLUDE_DIRS}
  ${ZMQPP_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${PROTOBUF_INCLUDE_DIRS}
)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS fooditem.proto)

# Add executable with all source files
add_executable(expirationTracker 
  main.cpp
  food_item.cpp
  endpoints.cpp
  logger.cpp
  messenger.cpp

  # Handler for messages from other processes and the display engine
  display/src/display_messenger.cpp

  # Database operations on food items
  display/src/sql_food.cpp
  
  # GUI 
  display/src/log_files.cpp
  display/src/engine_state.cpp
  display/src/display_engine.cpp
  display/src/sdl_debug.cpp
  display/src/display_global.cpp

  # GUI Elements
  display/src/elements/element_mediator.cpp
  display/src/elements/container.cpp
  display/src/elements/element.cpp
  display/src/elements/composite_element.cpp
  display/src/elements/button.cpp
  display/src/elements/text.cpp
  display/src/elements/panel.cpp
  display/src/elements/scroll_box.cpp
  display/src/elements/number_setting.cpp
  display/src/elements/dropdown.cpp
  display/src/elements/loading_bar.cpp

  # Game elements
  display/src/elements/flappy_food/flappy_food.cpp
  display/src/elements/flappy_food/bird.cpp
  display/src/elements/flappy_food/obstacle.cpp
  display/src/elements/flappy_food/obstacle_pair.cpp

  # GUI States
  display/src/states/state.cpp
  display/src/states/scanning.cpp
  display/src/states/item_list.cpp
  display/src/states/cancel_scan_confirmation.cpp
  display/src/states/scan_success.cpp
  display/src/states/scan_failure.cpp

  #Hardware process after forked from main
  hardware/src/hardware.cpp
  hardware/src/camera.cpp
  hardware/src/hardware_messenger.cpp

  ${PROTO_SRCS}
  ${PROTO_HDRS}
)

# Link all libraries
target_link_libraries(expirationTracker
  ${SDL2_LIBRARIES}
  SDL2_image
  SDL2_ttf
  ${OpenCV_LIBS}
  sqlite3
  ${ZMQ_LIBRARIES}
  ${ZMQPP_LIBRARIES}
  ${PROTOBUF_LIBRARIES}
  wiringPi
)

# Add library search directories if needed
link_directories(
  ${ZMQ_LIBRARY_DIRS}
  ${ZMQPP_LIBRARY_DIRS}
)
