@echo off

set SRC_DIR=src\engine\*.c src\game\server\*.c src\game\shared\*.c

set COMPILER_OPTS=/Fdbuild\x86\game\server\ /Febin\x86\game\server.exe /Fobuild\x86\game\server\ /Iinclude /Zi
set LINKER_OPTS=/libpath:lib\x86\ /subsystem:console
set LIBS=lib\x86\*.lib opengl32.lib

del bin\x86\game\server.exe
del bin\x86\game\server.ilk
del bin\x86\game\server.pdb

rd build\x86\game\server\ /s /q 
md build\x86\game\server\

cl %COMPILER_OPTS% %SRC_DIR% /link %LINKER_OPTS% %LIBS%

