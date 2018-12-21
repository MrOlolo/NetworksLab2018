cmake_minimum_required(VERSION 2.8)
project(server_linux)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pthread")

file(GLOB SOURCE "*.c" "socket/*.c" "product/*.c")
add_executable(server_linux ${SOURCE})
