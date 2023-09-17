project(ubctests)

add_executable(eof EXCLUDE_FROM_ALL eof.test.c)
set_target_properties(eof PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test/ubc")

add_executable(include EXCLUDE_FROM_ALL include.test.c)
set_target_properties(include PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test/ubc")

add_executable(nestedtypes EXCLUDE_FROM_ALL nestedtypes.test.c)
set_target_properties(nestedtypes PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test/ubc")

add_executable(unkownvar EXCLUDE_FROM_ALL unkownvar.test.c)
set_target_properties(unkownvar PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test/ubc")

add_custom_target(ubctests DEPENDS eof include nestedtypes unkownvar)