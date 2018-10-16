@echo off

set SRC_DIR=src\engine\*.c src\game\client\*.c src\game\shared\*.c

set COMPILER_OPTS=/Fdbuild\x86\game\client\ /Febin\x86\game\client.exe /Fobuild\x86\game\client\ /Iinclude /Zi
set LINKER_OPTS=/libpath:lib\x86\ /subsystem:console
set LIBS=lib\x86\*.lib opengl32.lib

del bin\x86\game\client.exe
del bin\x86\game\client.ilk
del bin\x86\game\client.pdb

rd build\x86\game\client\ /s /q 
md build\x86\game\client\

cl %COMPILER_OPTS% %SRC_DIR% /link %LINKER_OPTS% %LIBS%
