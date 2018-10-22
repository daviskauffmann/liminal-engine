@echo off

set SRC_DIR=src\*.c

set COMPILER_OPTS=/Fdbuild\x86\ /Febin\x86\game.exe /Fobuild\x86\ /I..\..\include\ /Iinclude\ /Zi
set LINKER_OPTS=/libpath:..\..\lib\x86\ /libpath:lib\x86\ /subsystem:console
set LIBS=engine.lib glew32.lib opengl32.lib SDL2_image.lib SDL2_mixer.lib SDL2.lib SDL2main.lib 

del bin\x86\game.exe
del bin\x86\game.ilk
del bin\x86\game.pdb

rd build\x86\ /s /q 
md build\x86\

copy ..\..\bin\x86\engine.dll bin\x86\
copy ..\..\bin\x86\engine.lib lib\x86\

cl %COMPILER_OPTS% %SRC_DIR% /link %LINKER_OPTS% %LIBS%
