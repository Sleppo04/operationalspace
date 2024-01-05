project(ubc C)

# Sources for server
target_sources(os-server PRIVATE parser.c
            tokenreader.c
            ubcvm.c)


# Ubcparser Library
add_library(ubcparser STATIC EXCLUDE_FROM_ALL
          parser.c)
target_include_directories(ubcparser BEFORE
          PRIVATE ../../common)

add_custom_target(ubclibs DEPENDS ubcparser)
