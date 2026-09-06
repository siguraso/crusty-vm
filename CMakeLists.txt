project(cava-sunday-vm C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -Werror")

set(SOURCES
    src/main.c
)

add_executable(cava-sunday-vm src/main.c)