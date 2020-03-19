cmake_minimum_required(VERSION 3.0)
project(can_transceiver C)

set(CMAKE_C_STANDARD 99)

add_compile_options(-Wall -Wextra -Wconversion -Wunused -Wshadow)

add_definitions(-Dtesting)
add_definitions(-Ddebug)

add_executable(can_transceiver main.c can_transceiver.h can_transceiver.c)
