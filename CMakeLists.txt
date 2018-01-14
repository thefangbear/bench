cmake_minimum_required(VERSION 3.9)
project(bench C)

if (APPLE)
    set(CMAKE_C_COMPILER /usr/local/bin/mpicc)
else ()
    set(CMAKE_C_COMPILER mpicc)
endif ()
set(CMAKE_C_STANDARD 11)

include(CMakeFiles/FindZeroMQ.cmake)
include_directories(bench ${LIBZMQ_INCLUDE_DIR})
if (NOT ${LIBZMQ_FOUND})
    message(FATAL_ERROR "Did not find libZMQ, aborting.")
endif ()

add_executable(bench main.c)
target_link_libraries(bench ${LIBZMQ_LIBRARIES})