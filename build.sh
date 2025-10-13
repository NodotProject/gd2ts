#!/bin/bash
# Build script for gd2ts GDExtension

set -e  # Exit on error

echo "=== Building gd2ts GDExtension ==="
echo

OUTPUT_PATH='addons/gd2ts'
GODOT_CPP_VERSION="godot-4.5-stable"
GODOT_CPP_RELEASE_URL="https://github.com/NodotProject/godot-cpp-builds/releases/download/${GODOT_CPP_VERSION}"

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

# Detect platform
detect_platform() {
    case "$(uname -s)" in
        Linux*)     echo "linux";;
        Darwin*)    echo "macos";;
        MINGW*|MSYS*|CYGWIN*)    echo "windows";;
        *)          echo "unknown";;
    esac
}

PLATFORM=$(detect_platform)

# Check for tree-sitter-gdscript submodule
if [ ! -d "tree-sitter-gdscript/src" ]; then
    echo -e "${RED}ERROR: tree-sitter-gdscript submodule not found.${NC}"
    echo -e "${RED}Run: git submodule update --init --recursive${NC}"
    exit 1
fi
echo -e "${GREEN}✓ tree-sitter-gdscript submodule found${NC}"

# Setup godot-cpp prebuilts
if [ -d "godot-cpp-prebuilt" ] && [ ! -d "godot-cpp" ]; then
    echo -e "${BLUE}Renaming godot-cpp-prebuilt to godot-cpp...${NC}"
    mv godot-cpp-prebuilt godot-cpp
    echo -e "${GREEN}✓ godot-cpp directory ready${NC}"
elif [ ! -d "godot-cpp" ]; then
    echo -e "${BLUE}Downloading godot-cpp prebuilt binaries (${GODOT_CPP_VERSION})...${NC}"

    # Download the prebuilt zip file
    ARCHIVE_NAME="godot-cpp-prebuilt-${GODOT_CPP_VERSION}.zip"
    DOWNLOAD_URL="${GODOT_CPP_RELEASE_URL}/${ARCHIVE_NAME}"

    echo "Downloading from: ${DOWNLOAD_URL}"

    if command -v wget &> /dev/null; then
        wget "${DOWNLOAD_URL}" -O "/tmp/${ARCHIVE_NAME}"
    elif command -v curl &> /dev/null; then
        curl -L "${DOWNLOAD_URL}" -o "/tmp/${ARCHIVE_NAME}"
    else
        echo -e "${RED}ERROR: Neither wget nor curl found. Install one of them.${NC}"
        exit 1
    fi

    # Check if unzip is installed
    if ! command -v unzip &> /dev/null; then
        echo -e "${RED}ERROR: unzip not found. Install with: sudo apt install unzip${NC}"
        exit 1
    fi

    echo -e "${BLUE}Extracting godot-cpp...${NC}"
    unzip -q "/tmp/${ARCHIVE_NAME}"
    rm "/tmp/${ARCHIVE_NAME}"

    echo -e "${GREEN}✓ godot-cpp prebuilts downloaded and extracted${NC}"
else
    echo -e "${GREEN}✓ godot-cpp directory already exists${NC}"
fi

# Build gd2ts
echo
echo -e "${BLUE}Building gd2ts extension...${NC}"

# Detect number of CPU cores for parallel build
get_cpu_count() {
    if command -v nproc &> /dev/null; then
        nproc
    elif command -v sysctl &> /dev/null; then
        sysctl -n hw.ncpu
    else
        echo "4"  # Default fallback
    fi
}

CPU_COUNT=$(get_cpu_count)

# Build debug version
echo "Building debug version..."
scons target=template_debug -j${CPU_COUNT}

# Build release version
echo "Building release version..."
scons target=template_release -j${CPU_COUNT}

echo
echo -e "${GREEN}=== Build Complete! ===${NC}"
echo
echo "Binaries location:"
ls -lh $OUTPUT_PATH/bin/*/*.so $OUTPUT_PATH/bin/*/*.dll $OUTPUT_PATH/bin/*/*.dylib 2>/dev/null || echo "  Check $OUTPUT_PATH/bin/ directory"