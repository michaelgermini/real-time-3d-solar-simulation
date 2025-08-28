param(
    [string]$SourceFile,
    [string]$OutputFile
)

$MINGW_PATH = "C:\msys64\mingw64"
$INCLUDE_PATH = "-I`"include`" -I`"$MINGW_PATH\include`""

Write-Host "Compiling $SourceFile..." -ForegroundColor Yellow
$result = & "$MINGW_PATH\bin\g++.exe" -std=c++17 -c $SourceFile $INCLUDE_PATH -o $OutputFile 2>&1

if ($LASTEXITCODE -ne 0) {
    Write-Host "Error compiling $($SourceFile):" -ForegroundColor Red
    Write-Host $result -ForegroundColor Red
    exit 1
} else {
    Write-Host "$SourceFile compiled successfully" -ForegroundColor Green
}
