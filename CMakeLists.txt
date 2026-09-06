cmake_minimum_required(VERSION 3.10)

project(cava-sunday-vm C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -Werror")

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/java_example/example.class
    ${CMAKE_CURRENT_BINARY_DIR}/java_example/example.class
    COPYONLY
)

set(SOURCES
    src/main.c
    src/class_file/class_file.c
)

add_executable(cava-sunday-vm ${SOURCES})