#!/bin/bash

echo "Building Real-Time 3D Simulation with OpenGL"
echo "============================================"

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    mkdir build
fi

cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release
if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

# Build the project using mingw32-make for MSYS2
echo "Building project..."
mingw32-make -j$(nproc)
if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo "Build completed successfully!"
echo ""
echo "To run the application:"
echo "  ./RealTime3DSimulation"
echo ""
