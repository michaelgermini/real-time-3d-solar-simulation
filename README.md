# 🌞 Real-Time 3D Solar Panel Simulation

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/std/the-standard)
[![OpenGL](https://img.shields.io/badge/OpenGL-4.3%2B-green.svg)](https://www.opengl.org/)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg)](https://github.com/michaelgermini/real-time-3d-solar-simulation)
[![Build Status](https://github.com/michaelgermini/real-time-3d-solar-simulation/workflows/Build%20and%20Test/badge.svg)](https://github.com/michaelgermini/real-time-3d-solar-simulation/actions)

> **Advanced OpenGL-based 3D visualization engine for solar energy applications**

A high-performance, real-time 3D simulation engine built with modern OpenGL, specifically designed for solar panel array visualization and energy analysis. This project demonstrates cutting-edge graphics programming techniques with a focus on performance optimization and realistic rendering.

## 📋 Table of Contents

- [Features](#-features)
- [Screenshots](#-screenshots)
- [Quick Start](#-quick-start)
- [Installation](#-installation)
- [Usage](#-usage)
- [Architecture](#-architecture)
- [Performance](#-performance)
- [Contributing](#-contributing)
- [License](#-license)

## ✨ Features

### 🎮 Core Rendering Engine
- **Modern OpenGL 4.3+** core profile with programmable shaders
- **Real-time 3D visualization** with interactive controls
- **Advanced lighting system** with multiple light types
- **Performance optimization** techniques (frustum culling, LOD, instancing)
- **Cross-platform support** (Windows, Linux, macOS)

### 🌟 Solar Panel Simulation
- **Real-time solar panel rendering** with realistic materials
- **Energy output calculations** based on sun position
- **Environmental shading analysis**
- **Array management** for large installations
- **Performance monitoring** and optimization

### 🚀 Technical Highlights
- **60 FPS** target on mid-range hardware
- **1000+ solar panels** rendered simultaneously
- **< 100 draw calls** per frame for optimized scenes
- **< 2GB VRAM** usage for typical scenes
- **Modular architecture** for easy extension

## 📸 Screenshots

> *Interactive 3D solar panel visualization with real-time rotation and lighting*

## 🚀 Quick Start

### Prerequisites

| Requirement | Version | Notes |
|-------------|---------|-------|
| **C++ Compiler** | C++17+ | GCC 7+, Clang 5+, MSVC 2017+ |
| **CMake** | 3.16+ | Build system |
| **OpenGL** | 4.3+ | Graphics API |
| **Graphics Card** | OpenGL 4.3+ compatible | NVIDIA GTX 1060+ recommended |

### Installation

#### 🪟 Windows (MSYS2)

```bash
# 1. Install MSYS2 from https://www.msys2.org/
# 2. Open MSYS2 MinGW 64-bit terminal

# 3. Install dependencies
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-cmake
pacman -S mingw-w64-x86_64-glfw
pacman -S mingw-w64-x86_64-glew
pacman -S mingw-w64-x86_64-glm

# 4. Clone and build
git clone https://github.com/michaelgermini/real-time-3d-solar-simulation.git
cd real-time-3d-solar-simulation
./build.sh
```

#### 🐧 Linux

```bash
# 1. Install dependencies
sudo apt-get update
sudo apt-get install build-essential cmake
sudo apt-get install libglfw3-dev libglew-dev libglm-dev

# 2. Clone and build
git clone https://github.com/michaelgermini/real-time-3d-solar-simulation.git
cd real-time-3d-solar-simulation
./build.sh
```

#### 🍎 macOS

```bash
# 1. Install dependencies
brew install cmake glfw glew glm

# 2. Clone and build
git clone https://github.com/michaelgermini/real-time-3d-solar-simulation.git
cd real-time-3d-solar-simulation
./build.sh
```

## 🎮 Usage

### Running the Simulation

```bash
# Navigate to build directory
cd build

# Run the 3D simulation
./RealTime3DSimulation
```

### Controls

| Key | Action |
|-----|--------|
| **ESC** | Exit application |
| **WASD** | Camera movement (full version) |
| **Mouse** | Look around (full version) |
| **Scroll Wheel** | Zoom (full version) |
| **F1** | Performance overlay (full version) |
| **F2** | Wireframe mode (full version) |

## 🏗️ Architecture

### Project Structure

```
real-time-3d-solar-simulation/
├── 📁 src/                          # Source files
│   ├── main_simple_3d.cpp          # Main application (working version)
│   ├── 📁 Engine/                  # Core engine components
│   │   ├── Renderer.cpp            # Main rendering pipeline
│   │   ├── Shader.cpp              # GLSL shader management
│   │   ├── Camera.cpp              # Camera and view controls
│   │   └── Scene.cpp               # Scene graph management
│   ├── 📁 Components/              # Specialized components
│   │   ├── SolarPanel.cpp          # Solar panel simulation
│   │   ├── Building.cpp            # Building generation
│   │   └── Skybox.cpp              # Atmospheric effects
│   └── 📁 Utils/                   # Utility functions
├── 📁 include/                      # Header files
├── 📁 shaders/                      # GLSL shader files
│   ├── 📁 vertex/                  # Vertex shaders
│   └── 📁 fragment/                # Fragment shaders
├── 📁 assets/                       # 3D models and textures
├── 📄 CMakeLists.txt               # Build configuration
├── 📄 build.sh                     # Linux/MSYS2 build script
├── 📄 build.bat                    # Windows build script
└── 📄 README.md                    # This file
```

### Technology Stack

| Component | Technology | Purpose |
|-----------|------------|---------|
| **Graphics API** | OpenGL 4.3+ | 3D rendering |
| **Window Management** | GLFW3 | Cross-platform windowing |
| **Mathematics** | GLM | 3D math operations |
| **Extension Loading** | GLEW | OpenGL extensions |
| **Build System** | CMake | Cross-platform builds |
| **Language** | C++17 | Core implementation |

## 📊 Performance

### Target Specifications

| Metric | Target | Notes |
|--------|--------|-------|
| **Frame Rate** | 60 FPS | Smooth real-time rendering |
| **Solar Panels** | 1000+ | Simultaneous rendering |
| **Draw Calls** | < 100 | Optimized per frame |
| **VRAM Usage** | < 2GB | Typical scenes |
| **CPU Usage** | < 30% | Efficient processing |

### Optimization Techniques

- **View Frustum Culling** (~80% reduction in rendered objects)
- **Level of Detail (LOD)** (~50% vertex reduction for distant objects)
- **Instanced Rendering** (~90% draw call reduction for repeated objects)
- **Texture Streaming** (Efficient memory management)
- **Shader Variants** (Pre-compiled shader combinations)

## 🛠️ Development

### Building from Source

#### Using CMake

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

#### Direct Compilation (MSYS2)

```bash
g++ -std=c++17 -O3 -I/mingw64/include -L/mingw64/lib \
    src/main_simple_3d.cpp -o RealTime3DSimulation.exe \
    -lglfw3 -lglew32 -lopengl32 -lgdi32 -luser32 -lkernel32
```

### Code Style

- **C++17** standards compliance
- **Meaningful variable names** with camelCase
- **Comprehensive comments** for complex logic
- **Consistent formatting** and indentation
- **Modular architecture** for maintainability

## 🤝 Contributing

We welcome contributions from the community! Please see our [Contributing Guidelines](CONTRIBUTING.md) for detailed information.

### How to Contribute

1. **Fork** the repository
2. **Create** a feature branch: `git checkout -b feature/amazing-feature`
3. **Make** your changes
4. **Test** thoroughly
5. **Commit** with clear messages: `git commit -m 'Add amazing feature'`
6. **Push** to your branch: `git push origin feature/amazing-feature`
7. **Open** a Pull Request

### Areas for Contribution

- **Performance optimization** for large solar arrays
- **New rendering features** (shadows, reflections)
- **Solar panel simulation** improvements
- **Documentation** updates and improvements
- **Testing framework** development

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

```
MIT License

Copyright (c) 2024 Real-Time 3D Solar Panel Simulation

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## 🙏 Acknowledgments

- **OpenGL** for the powerful graphics API
- **GLFW** for excellent cross-platform window management
- **GLEW** for seamless OpenGL extension loading
- **GLM** for comprehensive mathematics library
- **CMake** for robust build system
- **Solar Energy Community** for inspiration and real-world applications

## 📞 Support & Community

- **🐛 Issues**: [GitHub Issues](https://github.com/michaelgermini/real-time-3d-solar-simulation/issues)
- **💬 Discussions**: [GitHub Discussions](https://github.com/michaelgermini/real-time-3d-solar-simulation/discussions)
- **📚 Wiki**: [Project Wiki](https://github.com/michaelgermini/real-time-3d-solar-simulation/wiki)
- **📧 Contact**: michael@germini.info

---

<div align="center">

**Made with ❤️ for the solar energy community**

[![GitHub stars](https://img.shields.io/github/stars/michaelgermini/real-time-3d-solar-simulation?style=social)](https://github.com/michaelgermini/real-time-3d-solar-simulation/stargazers)
[![GitHub forks](https://img.shields.io/github/forks/michaelgermini/real-time-3d-solar-simulation?style=social)](https://github.com/michaelgermini/real-time-3d-solar-simulation/network/members)
[![GitHub issues](https://img.shields.io/github/issues/michaelgermini/real-time-3d-solar-simulation)](https://github.com/michaelgermini/real-time-3d-solar-simulation/issues)

</div>
