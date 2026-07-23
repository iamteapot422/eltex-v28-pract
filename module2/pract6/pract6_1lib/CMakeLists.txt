cmake_minimum_required(VERSION 4.3)
project(pract6_1lib C)

set(CMAKE_C_STANDARD 11)

add_library(pract6_1lib STATIC library.c
        phonebook.c
        phonebook.h
        stringlist.c
        stringlist.h
        tests.c
        tests.h)
