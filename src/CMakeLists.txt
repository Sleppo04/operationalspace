project(game)

add_executable(operational-space main.c)

target_link_libraries(operational-space m)

add_subdirectory(game)
add_subdirectory(logic)
add_subdirectory(ui)
