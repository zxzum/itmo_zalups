#!/bin/bash

# Build script for ITMO Loops GUI (macOS version)

set -e

echo "========================================="
echo "ITMO Loops GUI Build Script (macOS)"
echo "========================================="
echo ""

# Detect OS
if [[ "$OSTYPE" != "darwin"* ]]; then
    echo "Warning: This script is designed for macOS"
    echo "For Linux, use build_gui.sh instead"
    echo ""
fi

# Check for dependencies
echo "Checking dependencies..."

# Check for Homebrew
if ! command -v brew &> /dev/null; then
    echo "Error: Homebrew is not installed"
    echo "Install Homebrew first:"
    echo "  /bin/bash -c \"\$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\""
    exit 1
fi

# Check for CMake
if ! command -v cmake &> /dev/null; then
    echo "Error: CMake is not installed"
    echo "Install with: brew install cmake"
    exit 1
fi

# Check for compiler (clang on macOS)
if ! command -v clang++ &> /dev/null && ! command -v g++ &> /dev/null; then
    echo "Error: No C++ compiler found"
    echo "Install Xcode Command Line Tools with:"
    echo "  xcode-select --install"
    exit 1
fi

# Check for GLFW3
if ! pkg-config --exists glfw3 2>/dev/null && ! brew list glfw &> /dev/null; then
    echo "Warning: GLFW3 not found"
    echo "Install dependencies with:"
    echo "  brew install glfw"
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

# Build (use sysctl to get CPU count on macOS)
echo ""
echo "Building..."
if command -v sysctl &> /dev/null; then
    NCPU=$(sysctl -n hw.ncpu)
else
    NCPU=4
fi
make -j${NCPU} || { echo "Build failed"; exit 1; }

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
echo "Note: On macOS, you may need to grant accessibility"
echo "permissions for the GUI to run properly."
echo ""
