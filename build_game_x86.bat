@echo off

set SRC_DIR=src\game\*.c

set COMPILER_OPTS=/Fdbuild\x86\game\ /Febin\x86\game /Fobuild\x86\game\ /Iinclude /Zi
set LINKER_OPTS=/libpath:lib\x86\ /subsystem:console
set LIBS=lib\x86\*.lib opengl32.lib

del bin\x86\game.exe
del bin\x86\game.ilk
del bin\x86\game.pdb

rd build\x86\game /s /q 
md build\x86\game

cl %COMPILER_OPTS% %SRC_DIR% /link %LINKER_OPTS% %LIBS%
