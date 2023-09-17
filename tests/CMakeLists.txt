project(termtest)

add_executable(os-termtest
			ansitest.c
			../src/ui/window.c)
set_target_properties(os-termtest PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

add_executable(coordinates EXCLUDE_FROM_ALL coordinate.test.c)
set_target_properties(coordinates PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test")


add_executable(lexer EXCLUDE_FROM_ALL lexertest.c)
set_target_properties(lexer PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test")

add_executable(memorypool EXCLUDE_FROM_ALL memorypool.test.c)
set_target_properties(memorypool PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test")
