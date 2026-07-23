cmake_minimum_required(VERSION 4.3)
project(pract4_1 C)

set(CMAKE_C_STANDARD 11)

add_executable(pract4_1 main.c
        phonebook.c
        stringlist.c
        tests.c)
