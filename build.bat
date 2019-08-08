@echo off

set program=rmfast

set DEBUG=0
set INTERNAL=1
set RELEASE=1

set IgnoredWarnings=-wd4505
set CommonCompilerFlags=-nologo -Gm- -GR- -EHa- -Oi -WX -W4 -DAPP_WIN32=1 -FC -Z7 %IgnoredWarnings%
if %RELEASE% EQU 1 set DEBUG=0 && set INTERNAL=0
if %DEBUG% EQU 1 ( set CommonCompilerFlags=%CommonCompilerFlags% -Od
) ELSE ( set CommonCompilerFlags=%CommonCompilerFlags% -O2
)
if %INTERNAL% EQU 1 set CommonCompilerFlags=%CommonCompilerFlags% -DAPP_INTERNAL=1
set CommonLinkerFlags=-incremental:no -opt:ref

IF NOT EXIST build mkdir build
pushd build

cl %CommonCompilerFlags% ..\%program%.cpp /link %CommonLinkerFlags%
set builderror=%ERRORLEVEL%
popd

if %builderror% GTR 0 (
    exit /b %builderror%
)
