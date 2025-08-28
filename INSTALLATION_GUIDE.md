# Complete Installation Guide - 3D Solar Panel Simulation

## 🎯 **Goal: Get Your 3D Simulation Running**

This guide will walk you through installing MSYS2 and building your OpenGL 3D solar panel simulation.

## 📋 **Prerequisites Check**

✅ **Already Available:**
- CMake 4.1.1
- Complete project source code
- Build scripts and configuration

❌ **Need to Install:**
- MSYS2 (provides GCC compiler and OpenGL libraries)
- OpenGL development libraries

## 🚀 **Step 1: Install MSYS2**

### Option A: Automatic Installation
1. Run the installation script:
   ```bash
   .\install_msys2_packages.bat
   ```

### Option B: Manual Installation
1. **Download MSYS2** from: https://www.msys2.org/
2. **Run the installer** and install to `C:\msys64`
3. **Open MSYS2 MinGW 64-bit terminal** from Start Menu

## 🔧 **Step 2: Install Required Packages**

In the MSYS2 terminal that opened, run these commands one by one:

```bash
# Update package database
pacman -Syu

# Install C++ compiler
pacman -S mingw-w64-x86_64-gcc

# Install CMake
pacman -S mingw-w64-x86_64-cmake

# Install OpenGL libraries
pacman -S mingw-w64-x86_64-glfw
pacman -S mingw-w64-x86_64-glew
pacman -S mingw-w64-x86_64-glm

# Install additional tools
pacman -S mingw-w64-x86_64-make
pacman -S mingw-w64-x86_64-pkg-config
```

**Note:** When prompted, press `Y` to confirm installations.

## 🏗️ **Step 3: Build the Project**

In the MSYS2 terminal, navigate to your project and build:

```bash
# Navigate to your project directory
cd /c/Users/mika/Desktop/Real-Time\ 3D\ Simulation\ with\ OpenGL/

# Make build script executable
chmod +x build.sh

# Build the project
./build.sh
```

## 🎮 **Step 4: Run the 3D Simulation**

After successful build:

```bash
# Navigate to build directory
cd build

# Run the 3D simulation
./RealTime3DSimulation
```

## 🎯 **What You'll Experience**

### **3D Visualization Features:**
- Real-time solar panel array rendering
- Interactive camera controls (WASD + Mouse)
- Dynamic lighting and shadows
- Real-time energy calculations
- Performance monitoring

### **Controls:**
- **WASD**: Camera movement
- **Mouse**: Look around
- **Scroll Wheel**: Zoom
- **Space**: Move up
- **Shift**: Move down
- **F1**: Performance overlay
- **F2**: Wireframe mode
- **ESC**: Exit

## 🔍 **Troubleshooting**

### **If MSYS2 terminal doesn't open:**
1. Check if MSYS2 is installed in `C:\msys64`
2. Try opening manually from Start Menu: "MSYS2 MinGW 64-bit"

### **If packages fail to install:**
1. Update MSYS2 first: `pacman -Syu`
2. Try installing packages individually
3. Check internet connection

### **If build fails:**
1. Ensure all packages are installed
2. Check CMake configuration
3. Verify OpenGL libraries are found

### **If executable doesn't run:**
1. Check if all DLLs are available
2. Ensure graphics card supports OpenGL 4.3+
3. Try running from MSYS2 terminal

## 📊 **Expected Performance**

- **Target**: 60 FPS on mid-range hardware
- **Solar Panels**: 1000+ rendered simultaneously
- **Memory**: < 2GB VRAM usage
- **CPU**: Multi-core recommended

## 🎉 **Success Indicators**

✅ **Installation Complete When:**
- MSYS2 terminal opens successfully
- All packages install without errors
- Build completes with "Build completed successfully!"
- 3D simulation launches with interactive controls

## 📞 **Need Help?**

If you encounter issues:
1. Check the error messages carefully
2. Ensure all steps were followed exactly
3. Try the alternative Visual Studio approach
4. Check `BUILD_STATUS.md` for current status

---

**Your 3D solar panel simulation is ready to come to life!** 🌞⚡
