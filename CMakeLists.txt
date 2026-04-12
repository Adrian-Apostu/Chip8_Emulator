cmake_minimum_required(VERSION 3.30)
project(chip8_emulator C)

set(CMAKE_C_STANDARD 11)

find_package(SDL2 REQUIRED)

add_executable(chip8_emulator main.c chip8.c chip8.h)
target_link_libraries(chip8_emulator SDL2::SDL2)