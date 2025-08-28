@echo off
echo Building Real-Time 3D Simulation with OpenGL
echo ============================================

REM Create build directory if it doesn't exist
if not exist "build" mkdir build
cd build

REM Configure with CMake for Visual Studio 2022
echo Configuring with CMake...
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed!
    echo Trying alternative configuration...
    cmake .. -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Release
    if %ERRORLEVEL% neq 0 (
        echo All CMake configurations failed!
        pause
        exit /b 1
    )
)

REM Build the project
echo Building project...
cmake --build . --config Release
if %ERRORLEVEL% neq 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo Build completed successfully!
echo.
echo To run the application:
echo   cd build\Release
echo   RealTime3DSimulation.exe
echo.
pause
