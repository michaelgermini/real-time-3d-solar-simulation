@echo off
echo Building Real-Time 3D Simulation (Simple Version)
echo ================================================

REM Clean and create build directory
if exist "build" rmdir /s /q build
mkdir build
cd build

REM Copy the simplified CMakeLists to the current directory
copy ..\CMakeLists_simple.txt CMakeLists.txt

REM Configure with CMake using MinGW Makefiles
echo Configuring with CMake...
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% neq 0 (
    echo Trying Unix Makefiles...
    cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
    if %ERRORLEVEL% neq 0 (
        echo CMake configuration failed!
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
echo   cd build
echo   RealTime3DSimulation.exe
echo.
pause
