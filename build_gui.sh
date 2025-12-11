#!/bin/bash

# Build script for ITMO Loops GUI

set -e

echo "========================================="
echo "ITMO Loops GUI Build Script"
echo "========================================="
echo ""

# Check for dependencies
echo "Checking dependencies..."

if ! command -v cmake &> /dev/null; then
    echo "Error: CMake is not installed"
    echo "Install with: sudo apt-get install cmake"
    exit 1
fi

if ! command -v g++ &> /dev/null; then
    echo "Error: g++ is not installed"
    echo "Install with: sudo apt-get install build-essential"
    exit 1
fi

# Check for GLFW3
if ! pkg-config --exists glfw3; then
    echo "Warning: GLFW3 not found"
    echo "Install dependencies with:"
    echo "  sudo apt-get install libglfw3-dev libgl1-mesa-dev libglu1-mesa-dev xorg-dev"
    read -p "Continue anyway? (y/N) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

echo "Dependencies OK"
echo ""

# Create build directory
echo "Creating build directory..."
mkdir -p build
cd build

# Run CMake
echo ""
echo "Running CMake..."
cmake .. || { echo "CMake failed"; exit 1; }

# Build
echo ""
echo "Building..."
make -j$(nproc) || { echo "Build failed"; exit 1; }

echo ""
echo "========================================="
echo "Build successful!"
echo "========================================="
echo ""
echo "Run the GUI with:"
echo "  ./build/bin/itmoloops_gui"
echo ""
echo "Or from the build directory:"
echo "  cd build/bin && ./itmoloops_gui"
echo ""
