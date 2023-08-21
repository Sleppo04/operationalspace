#!/bin/sh

# Create bin directory
mkdir -p bin

# TODO: Make $scripting_language

# Make operationalspace executable
source_files="`find src/game -type f -name *.c` `find src/ui -type f -name *.c` `find src/logic -type f -name *.c` src/main.c"
library_files=`find . -type f -path *.a`
include_dirs=`find . -type d -path */include`
include_args=`for include in $include_dirs; do echo "-I$include "; done` 
CFLAGS="-std=c99 -pedantic -Wall -Wextra -ggdb"
DISABLED_BUG_WARNINGS="-Wno-missing-braces"
LIBRARIES="-lm -pthread"
gcc $source_files $library_files $include_args $CFLAGS $LIBRARIES $DISABLED_BUG_WARNINGS -o bin/operational-space

# Compile terminal test utility
gcc -x c $CFLAGS tests/ansitest.c.test src/ui/window.c -o bin/os-termtest
