@echo off
REM -------------------------------
REM Build & Run OpenGLTriangle
REM -------------------------------

REM Folder build
set BUILD_DIR=build

REM Capture optional parameters (GRID_N and SEED)
set GRID_N=%1
set SEED=%2

REM Utwórz folder build jeśli nie istnieje
if not exist %BUILD_DIR% (
    mkdir %BUILD_DIR%
)

REM Przejdź do build
cd %BUILD_DIR%

REM Wygeneruj pliki projektowe
cmake ..

REM Skompiluj projekt w trybie Release
cmake --build . --config Release

REM Uruchom wynikowy exe z opcjonalnymi argumentami
if exist Release\OpenGLTriangle.exe (
    echo Running OpenGLTriangle...
    if "%GRID_N%"=="" (
        Release\OpenGLTriangle.exe
    ) else if "%SEED%"=="" (
        Release\OpenGLTriangle.exe %GRID_N%
    ) else (
        Release\OpenGLTriangle.exe %GRID_N% %SEED%
    )
) else (
    echo ERROR: Executable not found!
)

REM Powrót do głównego folderu
cd ..
