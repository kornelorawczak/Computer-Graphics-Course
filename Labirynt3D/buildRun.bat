@echo off
REM Build and run script for OpenGL Maze Game
REM Usage: buildRun.bat [seed] [N]
REM   seed: optional random seed (default: random)
REM   N: optional grid size (default: 5)

setlocal

echo Creating build directory...
if not exist build mkdir build
cd build

echo Running CMake...
cmake ..

echo Building in Release mode...
cmake --build . --config Release

echo Running the game...
cd bin/Release
if "%1"=="" (
    if "%2"=="" (
        OpenGLMaze.exe
    ) else (
        OpenGLMaze.exe %2
    )
) else (
    if "%2"=="" (
        OpenGLMaze.exe %1
    ) else (
        OpenGLMaze.exe %1 %2
    )
)

cd ..\..
endlocal
pause