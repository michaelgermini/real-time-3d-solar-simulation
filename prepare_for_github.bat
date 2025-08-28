@echo off
echo Preparing project for GitHub...
echo =================================

echo.
echo Step 1: Cleaning build artifacts...
if exist "build" rmdir /s /q build
if exist "bin" rmdir /s /q bin
if exist "*.exe" del *.exe
if exist "*.o" del *.o
if exist "*.obj" del *.obj

echo.
echo Step 2: Removing temporary files...
if exist "*.tmp" del *.tmp
if exist "*.log" del *.log
if exist "*.cache" del *.cache

echo.
echo Step 3: Creating .gitignore if not exists...
if not exist ".gitignore" (
    echo Creating .gitignore file...
    copy nul .gitignore
)

echo.
echo Step 4: Checking for required files...
echo Checking README.md...
if exist "README.md" (
    echo ✓ README.md found
) else (
    echo ✗ README.md missing
)

echo Checking LICENSE...
if exist "LICENSE" (
    echo ✓ LICENSE found
) else (
    echo ✗ LICENSE missing
)

echo Checking .gitignore...
if exist ".gitignore" (
    echo ✓ .gitignore found
) else (
    echo ✗ .gitignore missing
)

echo Checking CONTRIBUTING.md...
if exist "CONTRIBUTING.md" (
    echo ✓ CONTRIBUTING.md found
) else (
    echo ✗ CONTRIBUTING.md missing
)

echo Checking CHANGELOG.md...
if exist "CHANGELOG.md" (
    echo ✓ CHANGELOG.md found
) else (
    echo ✗ CHANGELOG.md missing
)

echo Checking main source file...
if exist "src\main_simple_3d.cpp" (
    echo ✓ main_simple_3d.cpp found
) else (
    echo ✗ main_simple_3d.cpp missing
)

echo.
echo Step 5: Project structure summary...
echo.
echo Project files:
dir /b /s *.cpp *.h *.md *.txt *.bat *.sh 2>nul | findstr /v "build\\ bin\\"

echo.
echo Step 6: Ready for GitHub!
echo.
echo Next steps:
echo 1. Initialize git repository: git init
echo 2. Add all files: git add .
echo 3. Initial commit: git commit -m "Initial commit: Real-Time 3D Solar Panel Simulation"
echo 4. Create repository on GitHub
echo 5. Add remote: git remote add origin https://github.com/yourusername/real-time-3d-solar-simulation.git
echo 6. Push to GitHub: git push -u origin main
echo.
echo Project is ready for GitHub! 🌞⚡
pause
