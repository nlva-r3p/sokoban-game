cmake_minimum_required(VERSION 3.16)
project(Sokoban LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Require SFML 3 (Arch Linux pacman provides 3.0.1)
find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System Audio CONFIG)

# Main executable
add_executable(sokoban
  src/main.cpp
  src/Sokoban.cpp
)

# Public headers are in include/
target_include_directories(sokoban PRIVATE include)

target_link_libraries(sokoban PRIVATE
  SFML::Graphics
  SFML::Window
  SFML::System
  SFML::Audio
)

# Optionally copy assets into build dir for convenience
add_custom_command(TARGET sokoban POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/assets
          $<TARGET_FILE_DIR:sokoban>/assets)
