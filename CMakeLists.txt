cmake_minimum_required(VERSION 3.28.3)
project(chess)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -march=native -mtune=native -O3")
set(NUM_THREADS 8)
add_compile_definitions(NUM_THREADS=${NUM_THREADS})

# ----- Shared libraries (thread pool & chess engine) -----
add_library(thread_pool STATIC thread_pool.hpp thread_pool.cpp)
add_library(chess_core STATIC chess.hpp chess.cpp)

# chess_core depends on thread_pool
target_link_libraries(chess_core PUBLIC thread_pool)

# ----- Executables -----
add_executable(chess main.cpp)
target_link_libraries(chess PRIVATE chess_core)

add_executable(check tests.cpp)
target_link_libraries(check PRIVATE chess_core)