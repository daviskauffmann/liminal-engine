@echo off

set SRC_DIR=src\server\*.c src\shared\*.c

set COMPILER_OPTS=/Fdbuild\x86\server\ /Febin\x86\server.exe /Fobuild\x86\server\ /Iinclude /Zi
set LINKER_OPTS=/libpath:lib\x86\ /subsystem:console
set LIBS=lib\x86\*.lib opengl32.lib

del bin\x86\server.exe
del bin\x86\server.ilk
del bin\x86\server.pdb

rd build\x86\server\ /s /q 
md build\x86\server\

cl %COMPILER_OPTS% %SRC_DIR% /link %LINKER_OPTS% %LIBS%
