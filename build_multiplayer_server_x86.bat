@echo off

set SRC_DIR=src\engine\*.c src\multiplayer\server\*.c src\multiplayer\shared\*.c

set COMPILER_OPTS=/Fdbuild\x86\multiplayer\server\ /Febin\x86\multiplayer\server.exe /Fobuild\x86\multiplayer\server\ /Iinclude /Zi
set LINKER_OPTS=/libpath:lib\x86\ /subsystem:console
set LIBS=lib\x86\*.lib opengl32.lib

del bin\x86\multiplayer\server.exe
del bin\x86\multiplayer\server.ilk
del bin\x86\multiplayer\server.pdb

rd build\x86\multiplayer\server\ /s /q 
md build\x86\multiplayer\server\

cl %COMPILER_OPTS% %SRC_DIR% /link %LINKER_OPTS% %LIBS%

