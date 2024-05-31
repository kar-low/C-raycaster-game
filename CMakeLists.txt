cmake_minimum_required(VERSION 3.0.0)
project (rayLow VERSION 1.0.0)
cmake_policy(SET CMP0072 NEW)
set(USE_WAYLAND ON)
add_executable(rayLow 
  src/main.c
  src/textures.h
  )

target_link_libraries(rayLow raylib m)

