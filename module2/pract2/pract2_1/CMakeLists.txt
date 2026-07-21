cmake_minimum_required(VERSION 4.3)
project(pract2_1 C)

set(CMAKE_C_STANDARD 11)

add_executable(pract2_1 main.c
        tests.c
        phonebook.c
        phonebook.h)
