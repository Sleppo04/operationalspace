project(termtest)

add_executable(os-termtest
			ansitest.c
			../src/ui/window.c)
set_target_properties(os-termtest PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
