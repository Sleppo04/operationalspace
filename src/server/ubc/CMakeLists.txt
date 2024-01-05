project(ubc C)

# Sources for server
target_sources(os-server PRIVATE parser.c
            tokenreader.c
            ubcvm.c)


# Ubcparser Library
add_library(ubcparser STATIC EXCLUDE_FROM_ALL
          parser.c
          tokenreader.c
          "${COMMON_DIR}/util.c"
          "${COMMON_DIR}/lexer/lexer.c")
target_include_directories(ubcparser BEFORE
          PRIVATE "${COMMON_DIR}")
set_target_properties(ubcparser PROPERTIES
          ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/libs/ubc")

add_custom_target(ubclibs DEPENDS ubcparser)
