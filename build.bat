@echo off

set WarningFlags=-WX -W4 -wd4100 -wd4189 -wd4200 -wd4530
set CompilerFlags=-FC -GR- -EHa- -nologo -Zi %WarningFlags%
set SourceFiles=..\src\main.cc ..\src\lexer.cc  ..\src\tests.cc ..\src\common.cc ..\src\ast.cc

if not exist bin mkdir bin
pushd bin
cl %CompilerFlags% %SourceFiles% -Feuso.exe
popd
