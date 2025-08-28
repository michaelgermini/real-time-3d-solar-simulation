@echo off
echo Compiling Real-Time 3D Simulation...

set MINGW_PATH=C:\msys64\mingw64
set INCLUDE_PATH=-I"include" -I"%MINGW_PATH%\include"
set LIB_PATH=-L"%MINGW_PATH%\lib"
set LIBS=-lglfw3 -lglew32 -lassimp -lopengl32 -lkernel32 -luser32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32

echo Compiling main_3d.cpp...
"%MINGW_PATH%\bin\g++.exe" -std=c++17 -c src/main_3d.cpp %INCLUDE_PATH% -o main_3d.o
if errorlevel 1 (
    echo Error compiling main_3d.cpp
    pause
    exit /b 1
)

echo Compiling Shader.cpp...
"%MINGW_PATH%\bin\g++.exe" -std=c++17 -c src/Engine/Shader.cpp %INCLUDE_PATH% -o Shader.o
if errorlevel 1 (
    echo Error compiling Shader.cpp
    pause
    exit /b 1
)

echo Compiling Renderer.cpp...
"%MINGW_PATH%\bin\g++.exe" -std=c++17 -c src/Engine/Renderer.cpp %INCLUDE_PATH% -o Renderer.o
if errorlevel 1 (
    echo Error compiling Renderer.cpp
    pause
    exit /b 1
)

echo Compiling Camera.cpp...
"%MINGW_PATH%\bin\g++.exe" -std=c++17 -c src/Engine/Camera.cpp %INCLUDE_PATH% -o Camera.o
if errorlevel 1 (
    echo Error compiling Camera.cpp
    pause
    exit /b 1
)

echo Compiling Scene.cpp...
"%MINGW_PATH%\bin\g++.exe" -std=c++17 -c src/Engine/Scene.cpp %INCLUDE_PATH% -o Scene.o
if errorlevel 1 (
    echo Error compiling Scene.cpp
    pause
    exit /b 1
)

echo Compiling Light.cpp...
"%MINGW_PATH%\bin\g++.exe" -std=c++17 -c src/Engine/Light.cpp %INCLUDE_PATH% -o Light.o
if errorlevel 1 (
    echo Error compiling Light.cpp
    pause
    exit /b 1
)

echo Compiling Mesh.cpp...
"%MINGW_PATH%\bin\g++.exe" -std=c++17 -c src/Engine/Mesh.cpp %INCLUDE_PATH% -o Mesh.o
if errorlevel 1 (
    echo Error compiling Mesh.cpp
    pause
    exit /b 1
)

echo Compiling Model.cpp...
"%MINGW_PATH%\bin\g++.exe" -std=c++17 -c src/Engine/Model.cpp %INCLUDE_PATH% -o Model.o
if errorlevel 1 (
    echo Error compiling Model.cpp
    pause
    exit /b 1
)

echo Compiling Texture.cpp...
"%MINGW_PATH%\bin\g++.exe" -std=c++17 -c src/Engine/Texture.cpp %INCLUDE_PATH% -o Texture.o
if errorlevel 1 (
    echo Error compiling Texture.cpp
    pause
    exit /b 1
)

echo Compiling Skybox.cpp...
"%MINGW_PATH%\bin\g++.exe" -std=c++17 -c src/Components/Skybox.cpp %INCLUDE_PATH% -o Skybox.o
if errorlevel 1 (
    echo Error compiling Skybox.cpp
    pause
    exit /b 1
)

echo Compiling Building.cpp...
"%MINGW_PATH%\bin\g++.exe" -std=c++17 -c src/Components/Building.cpp %INCLUDE_PATH% -o Building.o
if errorlevel 1 (
    echo Error compiling Building.cpp
    pause
    exit /b 1
)

echo Compiling SolarPanel.cpp...
"%MINGW_PATH%\bin\g++.exe" -std=c++17 -c src/Components/SolarPanel.cpp %INCLUDE_PATH% -o SolarPanel.o
if errorlevel 1 (
    echo Error compiling SolarPanel.cpp
    pause
    exit /b 1
)

echo Compiling Landscape.cpp...
"%MINGW_PATH%\bin\g++.exe" -std=c++17 -c src/Components/Landscape.cpp %INCLUDE_PATH% -o Landscape.o
if errorlevel 1 (
    echo Error compiling Landscape.cpp
    pause
    exit /b 1
)

echo Linking...
"%MINGW_PATH%\bin\g++.exe" -o RealTime3DSimulation.exe main_3d.o Shader.o Renderer.o Camera.o Scene.o Light.o Mesh.o Model.o Texture.o Skybox.o Building.o SolarPanel.o Landscape.o %LIB_PATH% %LIBS%
if errorlevel 1 (
    echo Error linking
    pause
    exit /b 1
)

echo Compilation successful!
echo Copying DLLs...
copy "%MINGW_PATH%\bin\libgcc_s_seh-1.dll" .
copy "%MINGW_PATH%\bin\libstdc++-6.dll" .
copy "%MINGW_PATH%\bin\glew32.dll" .
copy "%MINGW_PATH%\bin\glfw3.dll" .
copy "%MINGW_PATH%\bin\libassimp-6.dll" .
copy "%MINGW_PATH%\bin\libwinpthread-1.dll" .

echo Done!
pause
