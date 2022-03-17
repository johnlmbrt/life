cmake_minimum_required(VERSION 3.22)

project(life)

include(compiler.cmake)
include(libraries.cmake)

add_library(console INTERFACE)
target_include_directories(console INTERFACE console)
target_link_libraries(console INTERFACE ncurses fmt)

add_library(display INTERFACE)
target_include_directories(display INTERFACE display)
target_link_libraries(display INTERFACE SDL2)

add_library(grid INTERFACE)
target_include_directories(grid INTERFACE grid)


add_executable(life)
target_sources(life PRIVATE life.cpp)
target_link_libraries(life PRIVATE 
  console
  display
  grid

  fmt
  czmq
)