project(termtest)

add_executable(os-termtest
			ansitest.c
			../src/ui/window.c)
set_target_properties(os-termtest PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

add_executable(coordinates EXCLUDE_FROM_ALL coordinate.test.c
					../src/logic/coordinate/coordinate.c
					../src/logic/arraylist/arraylist.c)
set_target_properties(coordinates PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test")


add_executable(lexer EXCLUDE_FROM_ALL lexertest.c 
					../src/logic/lexer/lexer.c)
set_target_properties(lexer PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test")

add_executable(memorypool EXCLUDE_FROM_ALL memorypool.test.c
					../src/logic/memorypool/memorypool.c
					../src/logic/memorypool/memoryarena.c)
set_target_properties(memorypool PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test")

add_executable(opensimplex EXCLUDE_FROM_ALL opensimplex.test.c)
set_target_properties(opensimplex PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test")
target_link_libraries(opensimplex m)

add_executable(thread EXCLUDE_FROM_ALL thread.test.c ../src/logic/threading/threading.c)
set_target_properties(thread PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test")

add_executable(worldgen EXCLUDE_FROM_ALL worldgen.test.c
					../src/logic/arraylist/arraylist.c
					../src/logic/coordinate/coordinate.c
					../src/game/world.c
					../src/game/worldgen.c
					../src/logic/rand/xoshiro256.c
					../src/logic/rand/opensimplex.c)
set_target_properties(worldgen PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test")
target_link_libraries(worldgen m)

add_executable(xoshiro EXCLUDE_FROM_ALL xoshiro.test.c)
set_target_properties(xoshiro PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test")

add_subdirectory(ubc/)

add_custom_target(tests DEPENDS ubctests os-termtest coordinates lexer memorypool opensimplex thread worldgen xoshiro)