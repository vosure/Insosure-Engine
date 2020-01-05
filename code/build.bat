@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
call cls

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

cl -nologo ..\code\Engine\Engine.cpp /LD /link -incremental:no -opt:ref /EXPORT:Start opengl32.lib gdi32.lib user32.lib kernel32.lib winmm.lib ..\code\glfw\lib\glfw3dll.lib
cl -nologo ..\code\Main.cpp 

popd