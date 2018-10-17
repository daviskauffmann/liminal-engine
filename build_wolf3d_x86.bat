@echo off

set SRC_DIR=src\engine\*.c src\wolf3d\*.c

set COMPILER_OPTS=/Fdbuild\x86\wolf3d\ /Febin\x86\wolf3d\wolf3d.exe /Fobuild\x86\wolf3d\ /Iinclude /Zi
set LINKER_OPTS=/libpath:lib\x86\ /subsystem:console
set LIBS=lib\x86\*.lib opengl32.lib

del bin\x86\wolf3d\wolf3d.exe
del bin\x86\wolf3d\wolf3d.ilk
del bin\x86\wolf3d\wolf3d.pdb

rd build\x86\wolf3d\ /s /q 
md build\x86\wolf3d\

cl %COMPILER_OPTS% %SRC_DIR% /link %LINKER_OPTS% %LIBS%
