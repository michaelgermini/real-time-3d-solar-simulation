@echo off
echo Setting up Development Environment for OpenGL 3D Simulation
echo ===========================================================
echo.

echo Step 1: Installing Visual Studio Build Tools...
echo This will install the C++ compiler and build tools.
echo.
winget install Microsoft.VisualStudio.2022.BuildTools
if %ERRORLEVEL% neq 0 (
    echo Visual Studio Build Tools installation failed or already installed.
    echo.
)

echo.
echo Step 2: Installing OpenGL Development Libraries...
echo.

echo Installing GLFW3...
winget install glfw3 2>nul
if %ERRORLEVEL% neq 0 (
    echo GLFW3 not found in winget. Will download manually.
    echo Please download GLFW from: https://www.glfw.org/download.html
    echo.
)

echo Installing GLEW...
winget install glew 2>nul
if %ERRORLEVEL% neq 0 (
    echo GLEW not found in winget. Will download manually.
    echo Please download GLEW from: http://glew.sourceforge.net/
    echo.
)

echo.
echo Step 3: Setting up MSYS2 (Alternative approach)...
echo.
echo If the above installations don't work, you can use MSYS2:
echo 1. Download MSYS2 from: https://www.msys2.org/
echo 2. Install it to C:\msys64
echo 3. Open MSYS2 shell and run:
echo    pacman -S mingw-w64-x86_64-gcc
echo    pacman -S mingw-w64-x86_64-cmake
echo    pacman -S mingw-w64-x86_64-glfw
echo    pacman -S mingw-w64-x86_64-glew
echo    pacman -S mingw-w64-x86_64-glm
echo.

echo Step 4: Testing the setup...
echo.
echo Testing CMake...
cmake --version
if %ERRORLEVEL% neq 0 (
    echo CMake not found! Please install CMake.
    echo.
)

echo.
echo Testing C++ compiler...
where cl >nul 2>&1
if %ERRORLEVEL% equ 0 (
    echo Visual Studio compiler found!
    echo.
) else (
    echo Visual Studio compiler not found in PATH.
    echo Please run the Developer Command Prompt or add to PATH.
    echo.
)

echo.
echo Setup completed!
echo.
echo Next steps:
echo 1. If you installed Visual Studio Build Tools, restart your terminal
echo 2. Run: build_vs2022.bat
echo 3. If that doesn't work, try: build_simple.bat
echo.
echo For manual installation:
echo - Download GLFW: https://www.glfw.org/download.html
echo - Download GLEW: http://glew.sourceforge.net/
echo - Download GLM: https://github.com/g-truc/glm
echo.
pause
