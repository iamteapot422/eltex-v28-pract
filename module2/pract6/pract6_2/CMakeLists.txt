cmake_minimum_required(VERSION 4.3)
project(pract6_2 C)

set(CMAKE_C_STANDARD 11)

add_executable(pract6_2 main.c)
target_include_directories(main PRIVATE libs/phonebook/source)