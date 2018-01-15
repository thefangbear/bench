cmake_minimum_required(VERSION 3.5)
project(bench C)

#if (APPLE)
#    set(CMAKE_C_COMPILER /usr/local/bin/mpicc)
#else ()
    set(CMAKE_C_COMPILER mpicc)
#endif ()
set(CMAKE_C_STANDARD 11)

include(CMakeFiles/FindZeroMQ.cmake)
message(WARNING ${LIBZMQ_LIBRARIES})
if (NOT ${LIBZMQ_FOUND})
    message(FATAL_ERROR "Did not find libZMQ, aborting.")
endif ()

add_executable(bench main.c)
target_link_libraries(bench ${LIBZMQ_LIBRARIES})