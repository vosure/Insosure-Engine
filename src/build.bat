@echo off

REM IF NOT EXIST "C:/dev/InsosureEngine/build" mkdir "C:/dev/InsosureEngine/build"
REM pushd "C:/dev/InsosureEngine/build"

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
call cls  REM Used for cleaning up terminal

cl -nologo "C:\dev\InsosureEngine\src\Engine\Main.cpp" /LD /link -incremental:no
cl -nologo "C:\dev\InsosureEngine\src\Sandbox.cpp"

popd