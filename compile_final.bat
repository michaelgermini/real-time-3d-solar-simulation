@echo off
echo Compiling Enhanced Solar Panel Simulation...
C:\msys64\mingw64\bin\g++.exe -std=c++17 -o enhanced_intermediate.exe enhanced_intermediate.cpp -I"C:\msys64\mingw64\include" -L"C:\msys64\mingw64\lib" -lglfw3 -lglew32 -lopengl32 -lkernel32 -luser32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32
if %ERRORLEVEL% EQU 0 (
    echo Compilation successful!
    if exist enhanced_intermediate.exe (
        echo Executable created: enhanced_intermediate.exe
    ) else (
        echo Warning: Executable not found
    )
) else (
    echo Compilation failed!
)
pause
