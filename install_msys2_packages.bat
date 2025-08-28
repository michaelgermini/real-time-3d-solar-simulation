@echo off
echo Installing MSYS2 Packages for OpenGL 3D Simulation
echo ===================================================

echo.
echo This script will open MSYS2 MinGW 64-bit terminal and install required packages.
echo.
echo Please run these commands in the MSYS2 terminal that opens:
echo.
echo pacman -S mingw-w64-x86_64-gcc
echo pacman -S mingw-w64-x86_64-cmake
echo pacman -S mingw-w64-x86_64-glfw
echo pacman -S mingw-w64-x86_64-glew
echo pacman -S mingw-w64-x86_64-glm
echo.
echo After installation, you can build the project with:
echo cd /c/Users/mika/Desktop/Real-Time\ 3D\ Simulation\ with\ OpenGL/
echo ./build.sh
echo.

REM Try to open MSYS2 MinGW 64-bit terminal
if exist "C:\msys64\mingw64.exe" (
    echo Opening MSYS2 MinGW 64-bit terminal...
    start "MSYS2 MinGW 64-bit" "C:\msys64\mingw64.exe"
) else if exist "C:\msys64\usr\bin\bash.exe" (
    echo Opening MSYS2 bash terminal...
    start "MSYS2" "C:\msys64\usr\bin\bash.exe"
) else (
    echo MSYS2 not found in C:\msys64
    echo Please install MSYS2 first from https://www.msys2.org/
    echo.
)

echo.
echo Installation Instructions:
echo =========================
echo 1. In the MSYS2 terminal that opens, run these commands:
echo.
echo    pacman -S mingw-w64-x86_64-gcc
echo    pacman -S mingw-w64-x86_64-cmake
echo    pacman -S mingw-w64-x86_64-glfw
echo    pacman -S mingw-w64-x86_64-glew
echo    pacman -S mingw-w64-x86_64-glm
echo.
echo 2. After installation, navigate to your project:
echo.
echo    cd /c/Users/mika/Desktop/Real-Time\ 3D\ Simulation\ with\ OpenGL/
echo.
echo 3. Build the project:
echo.
echo    ./build.sh
echo.
echo 4. Run the 3D simulation:
echo.
echo    ./build/RealTime3DSimulation.exe
echo.
pause
