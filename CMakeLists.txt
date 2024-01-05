cmake_minimum_required(VERSION 3.22)
project(operationalspace C)

set(CMAKE_C_STANDARD 99)

# Directories
SET(COMMON_DIR "${CMAKE_SOURCE_DIR}/src/common")
SET(SERVER_DIR "${CMAKE_SOURCE_DIR}/src/server")
SET(CLIENT_DIR "${CMAKE_SOURCE_DIR}/src/client")
SET(UBC_DIR "${CMAKE_SOURCE_DIR}/src/server/ubc")

# Activate warnings
if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall -Wextra")
endif()
if (MSVC)
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} /W4")
endif()

# Deactivate bugged warnings
if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-missing-braces")
endif()

# add_subdirectory(tests/)
add_subdirectory(src/)
add_subdirectory(tests/ubc)
