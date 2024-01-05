project(ubctests C)

add_custom_target(ubctests)


function(ubctestexecutable name)
  set("target_name" "ubctests-${name}")
  add_executable("${target_name}" EXCLUDE_FROM_ALL "${name}.test.c")
  target_include_directories("${target_name}" BEFORE PRIVATE "${UBC_DIR}" "${COMMON_DIR}")
  target_link_libraries("${target_name}" ubcparser)
  set_target_properties("${target_name}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test/ubc")
  set_target_properties("${target_name}" PROPERTIES RUNTIME_OUTPUT_NAME "${name}")
  add_dependencies(ubctests "${target_name}")
  unset("target_name")
endfunction()

file(GLOB ubctestfiles "*.test.c")

foreach(file ${ubctestfiles})
  string(REPLACE ".test.c" ""  "basename" "${file}")
  string(REGEX REPLACE "/.*/" "" "executable_name" "${basename}")
  ubctestexecutable("${executable_name}")
endforeach()
