@echo off

set SRC_DIR=src\engine\*.c src\multiplayer\client\*.c src\multiplayer\shared\*.c

set COMPILER_OPTS=/Fdbuild\x86\multiplayer\client\ /Febin\x86\multiplayer\client.exe /Fobuild\x86\multiplayer\client\ /Iinclude /Zi
set LINKER_OPTS=/libpath:lib\x86\ /subsystem:console
set LIBS=lib\x86\*.lib opengl32.lib

del bin\x86\multiplayer\client.exe
del bin\x86\multiplayer\client.ilk
del bin\x86\multiplayer\client.pdb

rd build\x86\multiplayer\client\ /s /q 
md build\x86\multiplayer\client\

cl %COMPILER_OPTS% %SRC_DIR% /link %LINKER_OPTS% %LIBS%
