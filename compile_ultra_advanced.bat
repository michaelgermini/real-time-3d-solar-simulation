@echo off
echo Compiling Ultra Advanced Solar Panel Simulation...
echo.

C:\msys64\mingw64\bin\g++.exe -std=c++17 -o ultra_advanced_3d.exe ultra_advanced_3d.cpp -I"C:\msys64\mingw64\include" -L"C:\msys64\mingw64\lib" -lglfw3 -lglew32 -lopengl32 -lkernel32 -luser32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32 2>&1

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Compilation successful!
    if exist ultra_advanced_3d.exe (
        echo Executable created: ultra_advanced_3d.exe
        echo Size: 
        dir ultra_advanced_3d.exe | find "ultra_advanced_3d.exe"
    ) else (
        echo Warning: Executable not found despite successful compilation
    )
) else (
    echo.
    echo Compilation failed with error code: %ERRORLEVEL%
)

echo.
pause
