cmake_minimum_required(VERSION 4.3)
project(pract6_1 C)

set(CMAKE_C_STANDARD 11)

add_executable(pract6_1 main.c)

target_link_libraries(pract6_1 PRIVATE
        ${CMAKE_SOURCE_DIR}/libpract6_1lib.a
)
