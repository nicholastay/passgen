@echo off
rem -- RUN FROM DEVELOPER CMD FOR VS!! --

rem /W4 used because /Wall is too much...
set CFLAGS=/W4 /FC /nologo
rem Link for wincrypt support
set LIBS=Advapi32.lib

cl %CFLAGS% passgen.c /link %LIBS%