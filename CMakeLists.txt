cmake_minimum_required(VERSION 3.10)

project(crutsy-vm C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -Werror")

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/java_example/example.class
    ${CMAKE_CURRENT_BINARY_DIR}/java_example/example.class
    COPYONLY
)

set(SOURCES
    src/main.c
    src/class_file_parser/class_file_parser.c
    src/instructions/start.c
    src/utils/utils.c
    src/instructions/opcodes.c
)

add_executable(crusty-vm ${SOURCES})