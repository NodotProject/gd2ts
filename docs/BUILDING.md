# Building gd2ts

This guide explains how to build the gd2ts GDExtension from source.

## Prerequisites

### All Platforms

- **Python 3.6+**: Required for SCons build system
- **SCons 4.0+**: Install via `pip install scons`
- **Git**: For cloning submodules

### Platform-Specific

#### Linux

```bash
# Ubuntu/Debian
sudo apt install build-essential python3-pip git

# Fedora
sudo dnf install gcc-c++ python3-pip git

# Install SCons
pip3 install scons
```

Compiler: GCC 7+ or Clang 7+

#### Windows

- **Visual Studio 2019+** with C++ development tools
- **Python 3.6+**: Download from python.org
- **SCons**: Install via `pip install scons`

Or use MinGW:
```bash
# Install MinGW-w64
# Then build with: scons use_mingw=true
```

#### macOS

```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install Python and SCons
brew install python scons
```

Requires Xcode 10+ for C++17 support.

## Building Step-by-Step

### 1. Clone the Repository

```bash
git clone --recursive https://github.com/yourname/gd2ts.git
cd gd2ts
```

If you already cloned without `--recursive`:

```bash
git submodule update --init --recursive
```

### 2. Build godot-cpp

The GDExtension depends on godot-cpp, which must be built first.

```bash
cd gdextension/godot-cpp

# Build debug version
scons target=template_debug

# Build release version
scons target=template_release

# Build for specific platform (if cross-compiling)
scons platform=linux target=template_release
scons platform=windows target=template_release
scons platform=macos target=template_release

cd ../..
```

**Note**: This step can take 10-20 minutes on first build.

### 3. Build gd2ts Extension

```bash
cd gdextension

# Build debug version (for development)
scons target=template_debug

# Build release version (for distribution)
scons target=template_release

# Build with specific options
scons target=template_debug platform=linux arch=x86_64
scons target=template_release use_llvm=true
```

### 4. Verify Build

Check that binaries were created:

```bash
ls -la bin/
```

You should see platform-specific directories with compiled libraries:
- Linux: `libgd2ts.linux.template_debug.x86_64.so`
- Windows: `libgd2ts.windows.template_debug.x86_64.dll`
- macOS: `libgd2ts.macos.template_debug.dylib`

## Build Options

### SCons Parameters

| Parameter | Description | Default | Options |
|-----------|-------------|---------|---------|
| `target` | Build type | `template_debug` | `editor`, `template_debug`, `template_release` |
| `platform` | Target platform | Auto-detect | `linux`, `windows`, `macos` |
| `arch` | CPU architecture | `x86_64` | `x86_32`, `x86_64`, `arm32`, `arm64` |
| `use_llvm` | Use LLVM/Clang | `false` | `true`, `false` |
| `use_mingw` | Use MinGW (Windows) | `false` | `true`, `false` |

### Examples

```bash
# Debug build for current platform
scons target=template_debug

# Release build for Linux x86_64
scons platform=linux target=template_release arch=x86_64

# Windows build using MinGW
scons platform=windows target=template_release use_mingw=true

# macOS universal binary (x86_64 + arm64)
scons platform=macos target=template_release
```

## Cross-Compilation

### Linux → Windows

Install MinGW cross-compiler:

```bash
sudo apt install mingw-w64
scons platform=windows target=template_release use_mingw=true
```

### Linux/macOS → Multiple Platforms

Build for all platforms:

```bash
#!/bin/bash
# build_all.sh

cd gdextension

# Linux
scons platform=linux target=template_release

# Windows (requires MinGW)
scons platform=windows target=template_release use_mingw=true

# macOS (requires macOS or OSX cross)
scons platform=macos target=template_release

echo "Build complete! Binaries in bin/"
```

## Troubleshooting

### godot-cpp Not Found

```
Error: godot-cpp/bin/libgodot-cpp.*.a not found
```

**Solution**: Build godot-cpp first (see step 2)

### tree-sitter Headers Not Found

```
Error: tree_sitter/api.h: No such file or directory
```

**Solution**: Initialize submodules

```bash
git submodule update --init --recursive
```

### C++17 Not Supported

```
Error: C++17 is required
```

**Solution**: Update your compiler
- Linux: GCC 7+ or Clang 7+
- Windows: Visual Studio 2019+
- macOS: Xcode 10+

### SCons Not Found

```
command not found: scons
```

**Solution**: Install SCons

```bash
pip install scons
# or
pip3 install scons
```

## Cleaning Build

```bash
cd gdextension

# Clean build artifacts
scons --clean

# Or manually
rm -rf bin/
rm -rf .scons*
rm -rf *.o *.os
```

## Development Builds

For faster iteration during development:

```bash
# Build only debug version
scons target=template_debug -j$(nproc)

# The -j flag enables parallel compilation
# $(nproc) uses all CPU cores
```

## Next Steps

After building:

1. Copy binaries to addon folder: `addons/gd2ts/bin/`
2. Copy GDExtension manifest: `gdextension/gdextension/gd2ts.gdextension` → `addons/gd2ts/`
3. Install addon in your Godot project
4. Enable plugin in Project Settings

See [README.md](../README.md) for usage instructions.
