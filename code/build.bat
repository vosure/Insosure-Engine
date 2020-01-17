set CommonCompilerFlags= -nologo
set CommonLinkerFlags= -incremental:no -opt:ref opengl32.lib gdi32.lib user32.lib kernel32.lib winmm.lib
set CommonOpenGLLinkerFlags= "..\dependencies\glfw\lib\glfw3dll.lib"
set IncludePaths= /I"W:/Insosure-Engine/dependencies"

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
call cls

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

XCOPY ..\dependencies\glfw\lib\glfw3.dll W:\Insosure-Engine\build /Y

cl %IncludePaths% %CommonCompilerFlags% ..\code\Engine\engine.cpp /Zi /LD /link %CommonLinkerFlags% /PDB:engine.pdb /DEBUG:FULL /EXPORT:Start %CommonOpenGLLinkerFlags%
cl %CommonCompilerFlags% ..\code\main.cpp /Zi

popd