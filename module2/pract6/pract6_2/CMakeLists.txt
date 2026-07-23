cmake_minimum_required(VERSION 4.3)
project(pract6_2 C)

set(CMAKE_C_STANDARD 11)

add_executable(pract6_2 main.c)

target_link_libraries(pract6_2 PRIVATE
${CMAKE_SOURCE_DIR}/libpract6_2lib.dll.a)

add_custom_command(TARGET pract6_2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/libpract6_2lib.dll
        $<TARGET_FILE_DIR:pract6_2>
)
