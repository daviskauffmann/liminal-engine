@echo off

set SRC_DIR=src\client\*.c src\shared\*.c

set COMPILER_OPTS=/Fdbuild\x86\client\ /Febin\x86\client.exe /Fobuild\x86\client\ /Iinclude /Zi
set LINKER_OPTS=/libpath:lib\x86\ /subsystem:console
set LIBS=lib\x86\*.lib opengl32.lib

del bin\x86\client.exe
del bin\x86\client.ilk
del bin\x86\client.pdb

rd build\x86\client\ /s /q 
md build\x86\client\

cl %COMPILER_OPTS% %SRC_DIR% /link %LINKER_OPTS% %LIBS%
