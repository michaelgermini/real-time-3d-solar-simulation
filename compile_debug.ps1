Write-Host "Compiling Real-Time 3D Simulation with detailed output..." -ForegroundColor Green

$MINGW_PATH = "C:\msys64\mingw64"
$INCLUDE_PATH = "-I`"include`" -I`"$MINGW_PATH\include`""
$LIB_PATH = "-L`"$MINGW_PATH\lib`""
$LIBS = "-lglfw3 -lglew32 -lassimp -lopengl32 -lkernel32 -luser32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32"

Write-Host "Compiling main_3d.cpp..." -ForegroundColor Yellow
$result = & "$MINGW_PATH\bin\g++.exe" -std=c++17 -c src/main_3d.cpp $INCLUDE_PATH -o main_3d.o 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "Error compiling main_3d.cpp:" -ForegroundColor Red
    Write-Host $result -ForegroundColor Red
    exit 1
}
Write-Host "main_3d.cpp compiled successfully" -ForegroundColor Green

Write-Host "Compiling Shader.cpp..." -ForegroundColor Yellow
$result = & "$MINGW_PATH\bin\g++.exe" -std=c++17 -c src/Engine/Shader.cpp $INCLUDE_PATH -o Shader.o 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "Error compiling Shader.cpp:" -ForegroundColor Red
    Write-Host $result -ForegroundColor Red
    exit 1
}
Write-Host "Shader.cpp compiled successfully" -ForegroundColor Green

Write-Host "Compiling Renderer.cpp..." -ForegroundColor Yellow
$result = & "$MINGW_PATH\bin\g++.exe" -std=c++17 -c src/Engine/Renderer.cpp $INCLUDE_PATH -o Renderer.o 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "Error compiling Renderer.cpp:" -ForegroundColor Red
    Write-Host $result -ForegroundColor Red
    exit 1
}
Write-Host "Renderer.cpp compiled successfully" -ForegroundColor Green

Write-Host "Compiling Camera.cpp..." -ForegroundColor Yellow
$result = & "$MINGW_PATH\bin\g++.exe" -std=c++17 -c src/Engine/Camera.cpp $INCLUDE_PATH -o Camera.o 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "Error compiling Camera.cpp:" -ForegroundColor Red
    Write-Host $result -ForegroundColor Red
    exit 1
}
Write-Host "Camera.cpp compiled successfully" -ForegroundColor Green

Write-Host "Compiling Scene.cpp..." -ForegroundColor Yellow
$result = & "$MINGW_PATH\bin\g++.exe" -std=c++17 -c src/Engine/Scene.cpp $INCLUDE_PATH -o Scene.o 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "Error compiling Scene.cpp:" -ForegroundColor Red
    Write-Host $result -ForegroundColor Red
    exit 1
}
Write-Host "Scene.cpp compiled successfully" -ForegroundColor Green

Write-Host "Compiling Light.cpp..." -ForegroundColor Yellow
$result = & "$MINGW_PATH\bin\g++.exe" -std=c++17 -c src/Engine/Light.cpp $INCLUDE_PATH -o Light.o 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "Error compiling Light.cpp:" -ForegroundColor Red
    Write-Host $result -ForegroundColor Red
    exit 1
}
Write-Host "Light.cpp compiled successfully" -ForegroundColor Green

Write-Host "Compiling Mesh.cpp..." -ForegroundColor Yellow
$result = & "$MINGW_PATH\bin\g++.exe" -std=c++17 -c src/Engine/Mesh.cpp $INCLUDE_PATH -o Mesh.o 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "Error compiling Mesh.cpp:" -ForegroundColor Red
    Write-Host $result -ForegroundColor Red
    exit 1
}
Write-Host "Mesh.cpp compiled successfully" -ForegroundColor Green

Write-Host "Compiling Model.cpp..." -ForegroundColor Yellow
$result = & "$MINGW_PATH\bin\g++.exe" -std=c++17 -c src/Engine/Model.cpp $INCLUDE_PATH -o Model.o 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "Error compiling Model.cpp:" -ForegroundColor Red
    Write-Host $result -ForegroundColor Red
    exit 1
}
Write-Host "Model.cpp compiled successfully" -ForegroundColor Green

Write-Host "Compiling Texture.cpp..." -ForegroundColor Yellow
$result = & "$MINGW_PATH\bin\g++.exe" -std=c++17 -c src/Engine/Texture.cpp $INCLUDE_PATH -o Texture.o 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "Error compiling Texture.cpp:" -ForegroundColor Red
    Write-Host $result -ForegroundColor Red
    exit 1
}
Write-Host "Texture.cpp compiled successfully" -ForegroundColor Green

Write-Host "Compiling Skybox.cpp..." -ForegroundColor Yellow
$result = & "$MINGW_PATH\bin\g++.exe" -std=c++17 -c src/Components/Skybox.cpp $INCLUDE_PATH -o Skybox.o 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "Error compiling Skybox.cpp:" -ForegroundColor Red
    Write-Host $result -ForegroundColor Red
    exit 1
}
Write-Host "Skybox.cpp compiled successfully" -ForegroundColor Green

Write-Host "Compiling Building.cpp..." -ForegroundColor Yellow
$result = & "$MINGW_PATH\bin\g++.exe" -std=c++17 -c src/Components/Building.cpp $INCLUDE_PATH -o Building.o 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "Error compiling Building.cpp:" -ForegroundColor Red
    Write-Host $result -ForegroundColor Red
    exit 1
}
Write-Host "Building.cpp compiled successfully" -ForegroundColor Green

Write-Host "Compiling SolarPanel.cpp..." -ForegroundColor Yellow
$result = & "$MINGW_PATH\bin\g++.exe" -std=c++17 -c src/Components/SolarPanel.cpp $INCLUDE_PATH -o SolarPanel.o 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "Error compiling SolarPanel.cpp:" -ForegroundColor Red
    Write-Host $result -ForegroundColor Red
    exit 1
}
Write-Host "SolarPanel.cpp compiled successfully" -ForegroundColor Green

Write-Host "Compiling Landscape.cpp..." -ForegroundColor Yellow
$result = & "$MINGW_PATH\bin\g++.exe" -std=c++17 -c src/Components/Landscape.cpp $INCLUDE_PATH -o Landscape.o 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "Error compiling Landscape.cpp:" -ForegroundColor Red
    Write-Host $result -ForegroundColor Red
    exit 1
}
Write-Host "Landscape.cpp compiled successfully" -ForegroundColor Green

Write-Host "Linking..." -ForegroundColor Yellow
$result = & "$MINGW_PATH\bin\g++.exe" -o RealTime3DSimulation.exe main_3d.o Shader.o Renderer.o Camera.o Scene.o Light.o Mesh.o Model.o Texture.o Skybox.o Building.o SolarPanel.o Landscape.o $LIB_PATH $LIBS 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "Error linking:" -ForegroundColor Red
    Write-Host $result -ForegroundColor Red
    exit 1
}
Write-Host "Linking successful!" -ForegroundColor Green

Write-Host "Compilation completed successfully!" -ForegroundColor Green
Write-Host "Copying DLLs..." -ForegroundColor Yellow
Copy-Item "$MINGW_PATH\bin\libgcc_s_seh-1.dll" .
Copy-Item "$MINGW_PATH\bin\libstdc++-6.dll" .
Copy-Item "$MINGW_PATH\bin\glew32.dll" .
Copy-Item "$MINGW_PATH\bin\glfw3.dll" .
Copy-Item "$MINGW_PATH\bin\libassimp-6.dll" .
Copy-Item "$MINGW_PATH\bin\libwinpthread-1.dll" .

Write-Host "Done! RealTime3DSimulation.exe is ready to run." -ForegroundColor Green
