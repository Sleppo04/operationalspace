#!/bin/sh
source_files=`find . -type f -path *.c`
library_files=`find . -type f -path *.a`
include_dirs=`find . -type d -path */include`
include_args=`for include in $include_dirs; do echo "-I$include "; done` 
CFLAGS="-Wall -Wextra -ggdb"
gcc $source_files $library_files $include_args $CFLAGS -o operational-space
