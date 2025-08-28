@echo off
echo Compiling Intermediate 3D with Click Functionality...
echo.

C:\msys64\mingw64\bin\g++.exe -std=c++17 -o intermediate_3d_click.exe intermediate_3d.cpp -I"C:\msys64\mingw64\include" -L"C:\msys64\mingw64\lib" -lglfw3 -lglew32 -lopengl32 -lkernel32 -luser32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32 2>&1

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Compilation successful!
    if exist intermediate_3d_click.exe (
        echo Executable created: intermediate_3d_click.exe
        echo Size: 
        dir intermediate_3d_click.exe | find "intermediate_3d_click.exe"
    ) else (
        echo Warning: Executable not found
    )
) else (
    echo.
    echo Compilation failed with error code: %ERRORLEVEL%
)

echo.
pause
