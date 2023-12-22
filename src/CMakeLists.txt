# Client
add_executable(operationalspace)
set_target_properties(operationalspace PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
target_link_libraries(operationalspace m)
target_include_directories(operationalspace PRIVATE client
            common)
add_subdirectory(client)

# Server
add_executable(os-server)
set_target_properties(os-server PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
target_link_libraries(os-server m)
target_include_directories(os-server PRIVATE server
            common)
add_subdirectory(server)

# Common
add_subdirectory(common)
