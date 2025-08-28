@echo off
echo Building Real-Time 3D Simulation with OpenGL (VS2022)
echo =====================================================

REM Set up Visual Studio 2022 environment
echo Setting up Visual Studio 2022 environment...
call "C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

REM Create build directory if it doesn't exist
if not exist "build" mkdir build
cd build

REM Configure with CMake
echo Configuring with CMake...
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed!
    pause
    exit /b 1
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
