@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
call cls

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

XCOPY ..\dependencies\glfw\lib\glfw3.dll D:\dev\InsosureEngine\build /Y
REM XCOPY ..\dependencies\glfw\lib\glfw3.dll ..\..\build /Y


cl -nologo ..\code\Engine\engine.cpp /LD /link -incremental:no -opt:ref /EXPORT:Start opengl32.lib gdi32.lib user32.lib kernel32.lib winmm.lib ..\dependencies\glfw\lib\glfw3dll.lib
cl -nologo ..\code\main.cpp 

PAUSE

popd