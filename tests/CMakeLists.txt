project(termtest C)

add_executable(os-termtest
			ansitest.c
			../src/client/ui/window.c)
set_target_properties(os-termtest PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

add_executable(coordinates EXCLUDE_FROM_ALL coordinate.test.c
					../src/common/coordinate/coordinate.c
					../src/common/arraylist/arraylist.c)
set_target_properties(coordinates PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test")


add_executable(lexer EXCLUDE_FROM_ALL lexertest.c
					../src/common/lexer/lexer.c)
set_target_properties(lexer PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test")

add_executable(memorypool EXCLUDE_FROM_ALL memorypool.test.c
					../src/common/memorypool/memorypool.c
					../src/common/memorypool/memoryarena.c)
set_target_properties(memorypool PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test")

add_executable(opensimplex EXCLUDE_FROM_ALL opensimplex.test.c
					../src/common/rand/opensimplex.c)
set_target_properties(opensimplex PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test")
target_link_libraries(opensimplex m)

add_executable(thread EXCLUDE_FROM_ALL thread.test.c ../src/common/threading/threading.c)
set_target_properties(thread PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test")

add_executable(worldgen EXCLUDE_FROM_ALL worldgen.test.c
					../src/common/arraylist/arraylist.c
					../src/common/coordinate/coordinate.c
					../src/server/world.c
					../src/server/worldgen.c
					../src/common/rand/xoshiro256.c
					../src/common/rand/opensimplex.c)
target_include_directories(worldgen BEFORE PRIVATE "${SERVER_DIR}" "${COMMON_DIR}")
set_target_properties(worldgen PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test")
target_link_libraries(worldgen m)

add_executable(xoshiro EXCLUDE_FROM_ALL xoshiro.test.c
					../src/common/rand/xoshiro256.c)
set_target_properties(xoshiro PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test")
target_link_libraries(xoshiro m)

add_subdirectory(ubc/)

add_custom_target(tests DEPENDS ubctests os-termtest coordinates lexer memorypool opensimplex thread worldgen xoshiro)
