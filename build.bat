@echo off

rem Uncomment this so windows can setup all needed paths and variables for the terminal
rem to build successfully. This will increase build times.
rem call "C:\programs\VisualStudio\VC\Auxiliary\Build\vcvars64.bat"

set SUPPRESSED_ERRORS= -wd4201 -wd4100 -wd4189 -wd4456 -wd4505
set COMMON_COMPILER_FLAGS= -MT -nologo -Gm- -GR- -EHsc -EHa- -Od -Oi -WX -W4 -Z7 -std:c++20
set COMMON_LINKER_FLAGS= -incremental:no -opt:ref

set TRIPLET=x64-windows-static
set INCLUDE_PATH="%VCPKG_INSTALLED%\installed\%TRIPLET%\include"
set LIB_PATH="%VCPKG_INSTALLED%\installed\%TRIPLET%\lib"

set ENGINE_INCLUDE_PATH="%cd%\source\engine"
set EXTERNAL_INCLUDE_PATH="%cd%\source\external"

set EXTERNAL= shell32.lib user32.lib gdi32.lib opengl32.lib glfw3.lib glew32.lib

if not exist build (
    mkdir build
    echo Creating Build folder...
)
pushd build

for /f %%i in ('powershell -NoProfile -Command "Get-Date -Format yyyyMMdd_HHmmss"') do set dateTime=%%i

del *.pdb > NUL 2> NUL
cl %COMMON_COMPILER_FLAGS% %SUPPRESSED_ERRORS% ..\source\example\game.cpp /I%INCLUDE_PATH% /I%ENGINE_INCLUDE_PATH% /Fmgame.map -LD /link /LIBPATH:%LIB_PATH% -incremental:no /PDB:game_%dateTime%.pdb
cl %COMMON_COMPILER_FLAGS% %SUPPRESSED_ERRORS% ..\source\engine\defacto.cpp /I%INCLUDE_PATH% /I%EXTERNAL_INCLUDE_PATH% /Fmdefacto.map /link /LIBPATH:%LIB_PATH% %EXTERNAL% %COMMON_LINKER_FLAGS%

popd
