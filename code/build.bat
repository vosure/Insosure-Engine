set CommonCompilerFlags= -nologo -DEBUG=1
set CommonLinkerFlags= -incremental:no -opt:ref opengl32.lib gdi32.lib user32.lib kernel32.lib winmm.lib "..\dependencies\freetype\lib\freetype.lib"
set CommonOpenGLLinkerFlags= "..\dependencies\glfw\lib\glfw3dll.lib"
set IncludePaths= /I"C:\dev\Insosure-Engine\dependencies" /I"C:\dev\Insosure-Engine\dependencies\freetype\include"

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
call cls

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

XCOPY ..\dependencies\glfw\lib\glfw3.dll C:\dev\Insosure-Engine\build /Y

cl %IncludePaths% %CommonCompilerFlags% ..\code\Engine\main.cpp /Zi /link %CommonLinkerFlags% %CommonOpenGLLinkerFlags%

popd