$MINGW_PATH = "C:\msys64\mingw64"
$INCLUDE_PATH = "-I`"$MINGW_PATH\include`""
$LIB_PATH = "-L`"$MINGW_PATH\lib`""
$LIBS = "-lglfw3 -lglew32 -lopengl32 -lkernel32 -luser32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32"

Write-Host "Compiling clickable_panels.cpp with detailed output..." -ForegroundColor Green

$result = & "$MINGW_PATH\bin\g++.exe" -std=c++17 -o clickable_panels.exe clickable_panels.cpp $INCLUDE_PATH $LIB_PATH $LIBS 2>&1

if ($LASTEXITCODE -ne 0) {
    Write-Host "Compilation failed with errors:" -ForegroundColor Red
    Write-Host $result -ForegroundColor Red
} else {
    Write-Host "Compilation successful!" -ForegroundColor Green
    if (Test-Path "clickable_panels.exe") {
        Write-Host "Executable created: clickable_panels.exe" -ForegroundColor Green
        $fileInfo = Get-Item "clickable_panels.exe"
        Write-Host "File size: $($fileInfo.Length) bytes" -ForegroundColor Green
    } else {
        Write-Host "Warning: Executable not found" -ForegroundColor Yellow
    }
}
