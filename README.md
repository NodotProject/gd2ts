# gd2ts - GDScript to TypeScript Transpiler

A Godot editor plugin that transpiles GDScript to TypeScript, powered by a high-performance C++ GDExtension.

## Overview

**gd2ts** is designed for game teams that use Godot as their editor but write game logic in TypeScript. With a simple button click in the editor, you can transpile GDScript files to TypeScript, making it easy to prototype in GDScript and then convert to your production language.

## Features

- **Editor Integration**: Convenient toolbar button for one-click transpilation
- **Fast Performance**: C++ core ensures quick transpilation even for large files
- **Tree-sitter Parser**: Uses the robust tree-sitter-gdscript parser for accurate parsing
- **Type Preservation**: Maintains type annotations from GDScript
- **Godot-Specific Support**: Handles signals, get_node(), exports, and other Godot constructs
- **Cross-Platform**: Works on Linux, Windows, and macOS

## Architecture

```
┌─────────────────────────┐
│   Godot Editor          │
│  ┌──────────────────┐   │
│  │ GDScript Plugin  │   │  ← User clicks transpile button
│  │  (UI Layer)      │   │
│  └────────┬─────────┘   │
│           │             │
│  ┌────────▼─────────┐   │
│  │  GDExtension     │   │  ← C++ transpiler core
│  │  (C++ Core)      │   │
│  └──────────────────┘   │
└─────────────────────────┘
```

## Project Status

**Phase 1: Foundation** ✅ (In Progress)
- [x] GDExtension project structure
- [x] godot-cpp submodule integration
- [x] GD2TSConverter class with Godot bindings
- [x] tree-sitter-gdscript integration
- [x] Basic parser wrapper
- [x] Test project setup
- [ ] Build and compilation (next step)

## Building

### Prerequisites

- Python 3.6+ (for SCons)
- SCons 4.0+
- C++17 compatible compiler:
  - Linux: GCC 7+ or Clang 7+
  - Windows: MSVC 2019+
  - macOS: Xcode 10+
- Git (for submodules)

### Build Steps

```bash
# Clone with submodules
git clone --recursive https://github.com/yourname/gd2ts.git
cd gd2ts

# Build godot-cpp first
cd gdextension/godot-cpp
scons target=template_debug
scons target=template_release
cd ../..

# Build gd2ts extension
cd gdextension
scons target=template_debug
scons target=template_release

# Binaries will be in gdextension/bin/
```

## Usage

### Installation

1. Copy the `addons/gd2ts` folder to your Godot project's `addons/` directory
2. Copy the compiled GDExtension binaries to `addons/gd2ts/bin/`
3. Enable the plugin in Project Settings → Plugins

### Using the Transpiler

#### From Editor

1. Click the "Transpile to TypeScript" button in the toolbar
2. Select GDScript file(s) or directory
3. Choose output location
4. Click "Transpile"

#### From Code

```gdscript
var converter = GD2TSConverter.new()

# Configure
converter.set_config({
    "inferTypes": true,
    "strictTypes": false,
    "preserveComments": true
})

# Transpile a single file
var result = converter.transpile_file(
    "res://scripts/player.gd",
    "res://typescript/player.ts"
)

if result["success"]:
    print("Transpiled successfully!")
else:
    print("Error: ", result["error"])

# Transpile directory
var results = converter.transpile_directory("res://scripts", {
    "outDir": "res://typescript"
})
```

## Configuration

See `addons/gd2ts/config/default_config.json` for available options:

- `inferTypes`: Attempt to infer types from context
- `strictTypes`: Fail on untyped variables
- `preserveComments`: Preserve comments from GDScript
- `convertSignals`: Convert signals to TypeScript patterns
- `convertGetNode`: Convert `$` and `get_node()` calls
- `verbose`: Enable verbose output

## Development

### Project Structure

```
gd2ts/
├── addons/gd2ts/          # Godot editor plugin
├── gdextension/           # C++ transpiler core
│   ├── src/              # Source code
│   ├── godot-cpp/        # Godot C++ bindings
│   └── thirdparty/       # tree-sitter-gdscript
├── tests/                # Test files
└── docs/                 # Documentation
```

### Contributing

Contributions are welcome! See [CONTRIBUTING.md](docs/CONTRIBUTING.md) for guidelines.

## License

[To be determined]

## Acknowledgments

- [tree-sitter-gdscript](https://github.com/PrestonKnopp/tree-sitter-gdscript) for the GDScript parser
- [godot-cpp](https://github.com/godotengine/godot-cpp) for GDExtension bindings
- Godot Engine team for the amazing game engine
