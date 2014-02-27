set CMAKE_EXE=W:\Apps\cmake-2.8.12.1-win32-x86\bin\cmake.exe

set OpenCV_VER=2.4.8
set OpenCV_DIR=W:\opencv-%OpenCV_VER%\build
set BUILD_DIR=bin-%OpenCV_VER%

pushd %~dp0

rd /s /q %BUILD_DIR%
mkdir %BUILD_DIR%
cd %BUILD_DIR%

%CMAKE_EXE% -G "Visual Studio 11 Win64" -DBUILD_SHARED_LIBS=OFF -DOpenCV_DIR=%OpenCV_DIR% ../src
pause
%CMAKE_EXE% --build . --config Release

popd
