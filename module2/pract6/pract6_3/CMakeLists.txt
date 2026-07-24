cmake_minimum_required(VERSION 4.3)
project(pract6_3 C)

set(CMAKE_C_STANDARD 11)

add_executable(pract6_3 main.c
        libs/addition/addition.c
        libs/addition/addition.h
        libs/division/division.c
        libs/division/division.h
        libs/multiplication/multiplication.c
        libs/multiplication/multiplication.h
        libs/substraction/substraction.c
        libs/substraction/substraction.h)
