@echo off
REM Build and run script for OpenGL Helicopter Game
REM Usage: buildRun.bat

setlocal

echo Creating build directory...
if not exist build mkdir build
cd build

echo Running CMake...
cmake ..

if %errorlevel% neq 0 (
    echo CMake configuration failed!
    cd ..
    pause
    exit /b %errorlevel%
)

echo Building in Release mode...
cmake --build . --config Release

if %errorlevel% neq 0 (
    echo Build failed!
    cd ..
    pause
    exit /b %errorlevel%
)

echo Copying shaders to runtime directory...
if not exist bin\Release\shaders mkdir bin\Release\shaders
xcopy /Y /I ..\shaders bin\Release\shaders\

echo Running the game...
cd bin\Release

if exist OpenGLHelicopterGame.exe (
    OpenGLHelicopterGame.exe
) else (
    echo Error: OpenGLHelicopterGame.exe not found!
    echo Available files in current directory:
    dir
)

cd ..\..
endlocal
pause