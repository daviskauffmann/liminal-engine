@echo off

set SRC_DIR=src\*.c

set COMPILER_OPTS=/DENGINE_EXPORTS /Fdbuild\x86\ /Febin\x86\engine.dll /Fobuild\x86\ /LD /Iinclude /Zi
set LINKER_OPTS=/libpath:lib\x86\
set LIBS=lib\x86\*.lib opengl32.lib

del bin\x86\engine.dll
del bin\x86\engine.exe
del bin\x86\engine.ilk
del bin\x86\engine.lib
del bin\x86\engine.pdb

rd build\x86\ /s /q 
md build\x86\

cl %COMPILER_OPTS% %SRC_DIR% /link %LINKER_OPTS% %LIBS%
