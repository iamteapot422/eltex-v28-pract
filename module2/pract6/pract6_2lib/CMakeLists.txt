cmake_minimum_required(VERSION 4.3)
project(pract6_2lib C)

set(CMAKE_C_STANDARD 11)

add_library(pract6_2lib SHARED library.c
phonebook.c
stringlist.c
tests.c
)
