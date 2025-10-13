#!/bin/bash
# Build script for gd2ts GDExtension

set -e  # Exit on error

echo "=== Building gd2ts GDExtension ==="
echo

OUTPUT_PATH='addons/gd2ts'

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Check if SCons is installed
if ! command -v scons &> /dev/null; then
    echo -e "${RED}ERROR: SCons not found. Install with: pip install scons${NC}"
    exit 1
fi

# Check if godot-cpp submodule is initialized
if [ ! -f "godot-cpp/SConstruct" ]; then
    echo -e "${BLUE}Initializing submodules...${NC}"
    git submodule update --init --recursive
fi

# Build godot-cpp
echo -e "${BLUE}Building godot-cpp...${NC}"
cd godot-cpp

# Check if already built
if [ ! -d "$OUTPUT_PATH/bin" ] || [ -z "$(ls -A $OUTPUT_PATH/bin 2>/dev/null)" ]; then
    echo "Building godot-cpp (this may take a while on first build)..."
    scons target=template_debug -j$(nproc)
    scons target=template_release -j$(nproc)
    echo -e "${GREEN}✓ godot-cpp built successfully${NC}"
else
    echo -e "${GREEN}✓ godot-cpp already built${NC}"
fi

cd ../..

# Build gd2ts
echo
echo -e "${BLUE}Building gd2ts extension...${NC}"

# Build debug version
echo "Building debug version..."
scons target=template_debug -j$(nproc)

# Build release version
echo "Building release version..."
scons target=template_release -j$(nproc)

cd ..

echo
echo -e "${GREEN}=== Build Complete! ===${NC}"
echo
echo "Binaries location:"
ls -lh $OUTPUT_PATH/bin/*/*.so $OUTPUT_PATH/bin/*/*.dll $OUTPUT_PATH/bin/*/*.dylib 2>/dev/null || echo "  Check $OUTPUT_PATH/bin/ directory"