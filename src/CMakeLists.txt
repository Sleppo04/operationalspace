project(game)

add_executable(operational-space main.c)

set_target_properties(operational-space PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

target_link_libraries(operational-space m)

add_subdirectory(game)
add_subdirectory(logic)
add_subdirectory(ui)
