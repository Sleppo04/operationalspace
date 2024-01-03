project(ubctests C)

add_executable(eof EXCLUDE_FROM_ALL eof.test.c
                ../../src/server/ubc/parser.c
                ../../src/common/lexer/lexer.c
                ../../src/server/ubc/tokenreader.c
                ../../src/common/util.c)
set_target_properties(eof PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test/ubc")

add_executable(include EXCLUDE_FROM_ALL include.test.c
                ../../src/server/ubc/parser.c
                ../../src/common/lexer/lexer.c
                ../../src/server/ubc/tokenreader.c
                ../../src/common/util.c)
set_target_properties(include PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test/ubc")

add_executable(nestedtypes EXCLUDE_FROM_ALL nestedtypes.test.c
                ../../src/server/ubc/parser.c
                ../../src/common/lexer/lexer.c
                ../../src/server/ubc/tokenreader.c
                ../../src/common/util.c)
set_target_properties(nestedtypes PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test/ubc")

add_executable(unknownvar EXCLUDE_FROM_ALL unknownvar.test.c
                ../../src/server/ubc/parser.c
                ../../src/common/lexer/lexer.c
                ../../src/server/ubc/tokenreader.c
                ../../src/common/util.c)
set_target_properties(unknownvar PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test/ubc")

add_executable(addition EXCLUDE_FROM_ALL addition.test.c
                ../../src/server/ubc/parser.c
                ../../src/common/lexer/lexer.c
                ../../src/server/ubc/tokenreader.c
                ../../src/common/util.c)
set_target_properties(addition PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test/ubc")

add_executable(literalmath EXCLUDE_FROM_ALL literalmath.test.c
                ../../src/server/ubc/parser.c
                ../../src/common/lexer/lexer.c
                ../../src/server/ubc/tokenreader.c
                ../../src/common/util.c)
set_target_properties(literalmath PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test/ubc")

add_executable(customtype EXCLUDE_FROM_ALL customtype.test.c
                ../../src/server/ubc/parser.c
                ../../src/common/lexer/lexer.c
                ../../src/server/ubc/tokenreader.c
                ../../src/common/util.c)
set_target_properties(customtype PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test/ubc")

add_executable(logic-and EXCLUDE_FROM_ALL logic-and.test.c
                ../../src/server/ubc/parser.c
                ../../src/common/lexer/lexer.c
                ../../src/server/ubc/tokenreader.c
                ../../src/common/util.c)
set_target_properties(logic-and PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test/ubc")


add_custom_target(ubctests DEPENDS eof include nestedtypes unknownvar addition literalmath customtype logic-and)
