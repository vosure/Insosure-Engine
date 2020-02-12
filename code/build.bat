set CommonCompilerFlags= -nologo -DEBUG=1
set CommonLinkerFlags= -incremental:no -opt:ref opengl32.lib gdi32.lib user32.lib kernel32.lib winmm.lib "..\dependencies\freetype\lib\freetype.lib" "..\dependencies\irrklang\lib\irrKlang.lib"
set CommonOpenGLLinkerFlags= "..\dependencies\glfw\lib\glfw3dll.lib"
set IncludePaths= /I"W:\Insosure-Engine\dependencies" /I"W:\Insosure-Engine\dependencies\freetype\include"

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
call cls

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

XCOPY ..\dependencies\glfw\lib\glfw3.dll W:\Insosure-Engine\build /Y
XCOPY ..\dependencies\freetype\lib\freetype.dll W:\Insosure-Engine\build /Y
XCOPY ..\dependencies\irrKlang\lib\irrKlang.dll W:\Insosure-Engine\build /Y
XCOPY ..\dependencies\irrKlang\lib\ikpMP3.dll W:\Insosure-Engine\build /Y

cl /EHsc %IncludePaths% %CommonCompilerFlags% ..\code\main.cpp /Zi /link %CommonLinkerFlags% %CommonOpenGLLinkerFlags%

popd