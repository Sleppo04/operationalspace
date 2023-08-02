@echo off
setlocal enabledelayedexpansion

mkdir bin

rem TODO: Make Wren

rem Make OperationalSpace executables
SET sourcefiles=
SET libraryfiles=
SET includeargs=
SET "includedirs="%CD%\include""
SET "cflags=-std=c99 -pedantic -Wall -Wextra -ggdb"
FOR /R src\game %%G IN (*.c) DO SET "sourcefiles=!sourcefiles! "%%G""
FOR /R src\ui %%G IN (*.c) DO SET "sourcefiles=!sourcefiles! "%%G""
FOR /R src\logic %%G IN (*.c) DO SET "sourcefiles=!sourcefiles! "%%G""
SET "sourcefiles=%sourcefiles% %CD%\src\main.c"
FOR /D %%G IN (%includedirs%) DO SET "includeargs=!includeargs! -I%%G"
gcc %cflags%%sourcefiles%%libraryfiles%%includeargs% -o bin\operational-space.exe

rem Make terminal test utility
gcc -x c %cflags% tests\ansitest.c.test src\ui\window.c -o bin\os-termtest.exe

endlocal