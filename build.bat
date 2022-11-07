@echo off

set WarningFlags=-WX -W4 -wd4100 -wd4189 -wd4200 -wd4530
set CompilerFlags=-FC -GR- -EHa- -nologo -Zi %WarningFlags%
set SourceFiles=..\src\main.cpp

if not exist bin mkdir bin
pushd bin
cl %CompilerFlags% %SourceFiles% -FeUso.exe
popd
