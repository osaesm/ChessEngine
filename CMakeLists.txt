cmake_minimum_required(VERSION 3.28.3)
project(chess)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -march=native -mtune=native -O3")
set(NUM_THREADS 8)
add_compile_definitions(NUM_THREADS=${NUM_THREADS})

# ----- Shared library (chess engine) -----
# Only chess.cpp is compiled once and reused
add_library(chess_core STATIC chess.hpp chess.cpp)

# ----- Executables -----
add_executable(chess main.cpp)
target_link_libraries(chess PRIVATE chess_core)

add_executable(check tests.cpp)
target_link_libraries(check PRIVATE chess_core)