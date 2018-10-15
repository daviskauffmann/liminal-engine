@echo off

set SRC_DIR=src\*.c src\engine\*.c src\game\*.c

set COMPILER_OPTS=/Fdbuild\x86\ /Febin\x86\program.exe /Fobuild\x86\ /Iinclude /Zi
set LINKER_OPTS=/libpath:lib\x86\ /subsystem:console
set LIBS=lib\x86\*.lib opengl32.lib

del bin\x86\*.exe
del bin\x86\*.ilk
del bin\x86\*.pdb

rd build\x86\ /s /q 
md build\x86\

cl %COMPILER_OPTS% %SRC_DIR% /link %LINKER_OPTS% %LIBS%
