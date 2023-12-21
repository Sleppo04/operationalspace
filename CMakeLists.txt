cmake_minimum_required(VERSION 3.22)
project(operationalspace)

set(CMAKE_C_STANDARD 99)

# Activate warnings
if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()
if (MSVC)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
endif()

# add_subdirectory(tests/)
add_subdirectory(src/)

