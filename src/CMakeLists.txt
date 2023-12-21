# Client
add_executable(operationalspace)
set_target_properties(operationalspace PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
target_link_libraries(operationalspace m)
add_subdirectory(client)
