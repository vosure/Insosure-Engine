@echo off

<<<<<<< HEAD
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
=======
set CommonLinkerFlags = -incremental:no -opt:ref opengl32.lib gdi32.lib user32.lib kernel32.lib winmm.lib 

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
>>>>>>> 442e3c63ce1f6c1d7c741b17ce67c6fa05665c59
call cls

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

XCOPY ..\dependencies\glfw\lib\glfw3.dll W:\InsosureEngine\build /Y

cl -nologo ..\code\Engine\engine.cpp /LD /link  /EXPORT:Start %CommonLinkerFlags% "..\dependencies\glfw\lib\glfw3dll.lib"
cl -nologo ..\code\main.cpp 

popd