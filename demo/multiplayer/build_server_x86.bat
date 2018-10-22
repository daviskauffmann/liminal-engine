@echo off

set SRC_DIR=src\server\*.c src\shared\*.c

set COMPILER_OPTS=/Fdbuild\x86\server\ /Febin\x86\server.exe /Fobuild\x86\server\ /I..\..\include\ /Iinclude\ /Zi
set LINKER_OPTS=/libpath:..\..\lib\x86\ /libpath:lib\x86\ /subsystem:console
set LIBS=engine.lib SDL2_net.lib SDL2.lib SDL2main.lib

del bin\x86\server.exe
del bin\x86\server.ilk
del bin\x86\server.pdb

rd build\x86\server\ /s /q 
md build\x86\server\

copy ..\..\bin\x86\engine.dll bin\x86\
copy ..\..\bin\x86\engine.lib lib\x86\

cl %COMPILER_OPTS% %SRC_DIR% /link %LINKER_OPTS% %LIBS%
