@echo off

set SRC_DIR=src\engine\*.c src\raycaster\*.c

set COMPILER_OPTS=/Fdbuild\x86\raycaster\ /Febin\x86\raycaster\raycaster.exe /Fobuild\x86\raycaster\ /Iinclude /Zi
set LINKER_OPTS=/libpath:lib\x86\ /subsystem:console
set LIBS=lib\x86\*.lib opengl32.lib

del bin\x86\raycaster\raycaster.exe
del bin\x86\raycaster\raycaster.ilk
del bin\x86\raycaster\raycaster.pdb

rd build\x86\raycaster\ /s /q 
md build\x86\raycaster\

cl %COMPILER_OPTS% %SRC_DIR% /link %LINKER_OPTS% %LIBS%
