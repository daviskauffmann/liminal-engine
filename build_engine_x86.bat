@echo off

set SRC_DIR=src\engine\*.c

set COMPILER_OPTS=/DENGINE_EXPORTS /Fdbuild\x86\engine\ /Febin\x86\engine /Fobuild\x86\engine\ /LD /Iinclude /Zi
set LINKER_OPTS=/libpath:lib\x86\
set LIBS=lib\x86\*.lib opengl32.lib

del bin\x86\engine.dll
del bin\x86\engine.exe
del bin\x86\engine.ilk
del bin\x86\engine.lib
del bin\x86\engine.pdb

rd build\x86\engine /s /q 
md build\x86\engine

cl %COMPILER_OPTS% %SRC_DIR% /link %LINKER_OPTS% %LIBS%

copy bin\x86\engine.lib lib\x86
