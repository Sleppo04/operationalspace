project(ubctests)

add_executable(eof EXCLUDE_FROM_ALL eof.test.c
                ../../src/logic/ubc/parser.c
                ../../src/logic/lexer/lexer.c
                ../../src/logic/ubc/tokenreader.c
                ../../src/logic/util.c)
set_target_properties(eof PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test/ubc")

add_executable(include EXCLUDE_FROM_ALL include.test.c
                ../../src/logic/ubc/parser.c
                ../../src/logic/lexer/lexer.c
                ../../src/logic/ubc/tokenreader.c
                ../../src/logic/util.c)
set_target_properties(include PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test/ubc")

add_executable(nestedtypes EXCLUDE_FROM_ALL nestedtypes.test.c
                ../../src/logic/ubc/parser.c
                ../../src/logic/lexer/lexer.c
                ../../src/logic/ubc/tokenreader.c
                ../../src/logic/util.c)
set_target_properties(nestedtypes PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test/ubc")

add_executable(unkownvar EXCLUDE_FROM_ALL unkownvar.test.c
                ../../src/logic/ubc/parser.c
                ../../src/logic/lexer/lexer.c
                ../../src/logic/ubc/tokenreader.c
                ../../src/logic/util.c)
set_target_properties(unkownvar PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test/ubc")

add_executable(addition EXCLUDE_FROM_ALL addition.test.c
                ../../src/logic/ubc/parser.c
                ../../src/logic/lexer/lexer.c
                ../../src/logic/ubc/tokenreader.c
                ../../src/logic/util.c)
set_target_properties(addition PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test/ubc")

add_executable(literalmath EXCLUDE_FROM_ALL literalmath.test.c
                ../../src/logic/ubc/parser.c
                ../../src/logic/lexer/lexer.c
                ../../src/logic/ubc/tokenreader.c
                ../../src/logic/util.c)
set_target_properties(literalmath PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test/ubc")

add_executable(customtype EXCLUDE_FROM_ALL customtype.test.c
                ../../src/logic/ubc/parser.c
                ../../src/logic/lexer/lexer.c
                ../../src/logic/ubc/tokenreader.c
                ../../src/logic/util.c)
set_target_properties(customtype PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test/ubc")


add_custom_target(ubctests DEPENDS eof include nestedtypes unkownvar addition literalmath customtype)