cmake_minimum_required(VERSION 3.16)
project(can_transceiver C)

set(CMAKE_C_STANDARD 99)

add_compile_options(-Wall -Wextra -Wconversion -Wunused -Wduplicated-branches -Wduplicated-cond -Wshadow)

add_executable(can_transceiver main.c can_transceiver.h can_transceiver.c)