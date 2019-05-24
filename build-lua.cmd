@echo off

setlocal
cd lib\lua

if "%VS140COMNTOOLS%" == "" (
  echo Environment variable VS140COMNTOOLS is not defined.
  exit /b 1
)
call "%VS140COMNTOOLS%\..\..\VC\vcvarsall" amd64

del *.o *.obj 2>nul
cl /nologo /MD /Od /c /DLUA_BUILD_AS_DLL /Zi *.c
ren lua.obj lua.o
rem ren luac.obj luac.o
link /nologo /DLL /IMPLIB:lua5.3.lib /OUT:lua5.3.dll *.obj
link /nologo /OUT:lua.exe lua.o lua5.3.lib
lib /nologo /OUT:lua5.3-static.lib *.obj
rem link /OUT:luac.exe luac.o lua5.3-static.lib

del *.o *.obj 2>nul
cl /nologo /MDd /Od /c /DLUA_BUILD_AS_DLL /Zi *.c
mv lua.obj lua.o
rem mv luac.obj luac.o
lib /nologo /OUT:lua5.3-staticd.lib *.obj
