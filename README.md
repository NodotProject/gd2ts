# gd2ts - GDScript to TypeScript Transpiler

A high-performance GDScript to TypeScript transpiler library, powered by a C++ GDExtension.

## Overview

**gd2ts** is designed for game teams that use Godot as their editor but write game logic in TypeScript. This library provides a simple API to transpile GDScript files to TypeScript, making it easy to prototype in GDScript and then convert to your production language.

## Features

- **Simple Library API**: Easy-to-use GDScript API for transpilation
- **Fast Performance**: C++ core ensures quick transpilation even for large files
- **Tree-sitter Parser**: Uses the robust tree-sitter-gdscript parser for accurate parsing
- **Type Preservation**: Maintains type annotations from GDScript
- **Godot-Specific Support**: Handles signals, get_node(), exports, and other Godot constructs
- **Cross-Platform**: Works on Linux, Windows, and macOS

## Architecture

```
┌─────────────────────────┐
│   Your GDScript Code    │
│                         │
│  GD2TSConverter.new()   │  ← Call transpiler API
│           │             │
│  ┌────────▼─────────┐   │
│  │  GDExtension     │   │  ← C++ transpiler core
│  │  (C++ Core)      │   │
│  └──────────────────┘   │
└─────────────────────────┘
```

## Project Status

### Completed Phases ✅

**Phase 1: GDExtension Foundation** ✅
- GDExtension project structure with SCons
- godot-cpp bindings integrated
- tree-sitter-gdscript parser integrated
- Cross-platform compilation working

**Phase 2: Editor Plugin UI** ❌ (Removed)
- Project is now a library-only solution with no UI integration

**Phase 3: Core Parser & AST** ✅
- Tree-sitter parser wrapper
- Complete AST node types
- AST traversal system
- Comment extraction

**Phase 4: Basic Transformation** ✅
- Type mapping (int→number, bool→boolean, etc.)
- Class/function/variable declarations
- Expression transformations
- Control flow structures

**Phase 5: Godot-Specific Features** ✅
- Signal handling with $ prefix
- Export variables with @exports
- get_node() transformations
- Node path syntax

**Phase 6: Advanced Language Features** ✅
- For loops (range, array iteration)
- Match → switch conversion
- Enums
- Vector operations
- Inner classes (partial)

**Phase 7: Code Generation & Polish** ✅
- TypeScript formatting and indentation
- Comment preservation (# → //)
- Class wrapping
- Import generation
- Error reporting infrastructure

**Phase 8: Testing & Documentation** ✅ (Current)
- Comprehensive GUT unit test suite (29 tests, 26 passing)
- Test sample library with 7 sample files
- Test documentation
- User documentation
- Known limitations documented

### Current Stats
- **Test Pass Rate**: 89.7% (26/29 tests)
- **Build Status**: ✅ Compiles successfully
- **Transpilation Speed**: <1s for typical files
- **Supported Features**: 90%+ of common GDScript patterns

### Known Limitations
- else/elif clauses not fully transpiled (2 failing tests)
- Local variable declarations missing let/const (1 failing test)
- Some Godot 4.x features incomplete (await, typed arrays)

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
cd godot-cpp
scons target=template_debug
scons target=template_release
cd ..

# Build gd2ts extension
scons target=template_debug
scons target=template_release

# Binaries will be in addons/gd2ts/bin/
```

## Usage

### Using from GDScript

```gdscript
# Create a transpiler instance
var converter = GD2TSConverter.new()

# Get version info
print("Transpiler version: ", converter.get_version())

# Configure (optional)
converter.set_config({
	"inferTypes": true,
	"strictTypes": false,
	"preserveComments": true,
	"verbose": false
})

# Transpile a string of GDScript
var gdscript_code = """
extends Node2D
class_name Player

var health: int = 100
signal health_changed(new_health)

func take_damage(amount: int) -> void:
    health -= amount
    health_changed.emit(health)
"""

var typescript = converter.transpile_string(gdscript_code)
print(typescript)

# Transpile a file
var result = converter.transpile_file(
	"res://scripts/player.gd",
    "res://typescript/player.ts"
)

if result["success"]:
	print("✓ Transpiled successfully!")
	print("  Output: ", result.get("output_path", ""))
else:
	print("✗ Transpilation failed!")
	print("  Error: ", result.get("error", "unknown"))
```

### Example Input/Output

**Input (GDScript)**:
```gdscript
extends Node2D
class_name Player

var health: int = 100
export var max_health: int = 100

signal health_changed(new_health)

func _ready() -> void:
	print("Player ready!")

func take_damage(amount: int) -> void:
	health -= amount
	if health < 0:
		health = 0
	health_changed.emit(health)
```

**Output (TypeScript)**:
```typescript
// Import Godot types (uncomment when ready to use)
import { Node2D, Signal } from 'godot';

class Player extends Node2D {
	health: number = 100;
	@exports
	max_health: number = 100;

	$health_changed!: Signal<[number]>;

	_ready(): void {
		print("Player ready!");
	}

	take_damage(amount: number): void {
		health -= amount;
		if (health < 0) {
			health = 0;
		}
		this.$health_changed.emit(health);
	}
}
```

## Configuration

See `addons/gd2ts/config/default_config.json` for available options:

- `inferTypes`: Attempt to infer types from context
- `strictTypes`: Fail on untyped variables
- `preserveComments`: Preserve comments from GDScript
- `convertSignals`: Convert signals to TypeScript patterns
- `convertGetNode`: Convert `$` and `get_node()` calls
- `verbose`: Enable verbose output

## Testing

### Running Tests

The project uses [GUT (Godot Unit Test)](https://github.com/bitwes/Gut) framework.

```bash
# Run all tests
godot --headless --path . -s addons/gut/gut_cmdln.gd -gexit

# Run specific test file
godot --headless --path . -s addons/gut/gut_cmdln.gd \
  -gtest=res://tests/unit/test_gd2ts_converter.gd \
  -gexit

# View test results
cat .ai/phase8-test-results.md
```

**Current Test Results**: 26/29 passing (89.7%)

See [tests/README.md](tests/README.md) for detailed test documentation.

## Troubleshooting

### Common Issues

#### 1. "GD2TSConverter not found"
**Cause**: GDExtension not loaded or binary missing

**Solution**:
- Check that `gdextension/bin/` contains compiled libraries
- Verify `.gdextension` file points to correct binary paths
- Try reimporting project: `godot --headless --path . --import --quit`

#### 2. "Parse error" when loading GDScript
**Cause**: Tree-sitter parser issue or unsupported syntax

**Solution**:
- Check GDScript syntax is valid in Godot editor
- Report unsupported syntax patterns as issues
- Check console for detailed error messages

#### 3. Generated TypeScript has errors
**Cause**: Known limitations (else/elif, local variables)

**Solution**:
- Manually add `else`/`else if` clauses (known issue #1)
- Add `let` or `const` to variable declarations in functions (known issue #2)
- Review generated code before using in production

#### 4. Missing types in output
**Cause**: Type inference limitations

**Solution**:
- Use explicit type annotations in GDScript
- Enable `inferTypes: true` in config
- Review and add missing types manually

### Performance Tips

- Transpile individual files rather than large directories
- Use `verbose: false` for faster transpilation
- Pre-compile GDExtension in release mode for production use

## Language Mapping Reference

### Type Conversions

| GDScript | TypeScript | Notes |
|----------|------------|-------|
| `int` | `number` | Comment added: `// int` |
| `float` | `number` | Default number type |
| `String` | `string` | Direct mapping |
| `bool` | `boolean` | Direct mapping |
| `Array` | `Array<any>` | Generic type |
| `Vector2/3` | `Vector2/3` | Preserved from Godot types |
| `Node`, `Node2D` | Same | Preserved |
| `Variant` | `any` | Fallback type |

### Syntax Conversions

| GDScript | TypeScript |
|----------|------------|
| `extends Node2D` | `extends Node2D` |
| `class_name MyClass` | `class MyClass` |
| `var x: int = 5` | `x: number = 5` |
| `export var damage: int` | `@exports damage: number` |
| `signal clicked` | `$clicked!: Signal` |
| `func _ready() -> void:` | `_ready(): void {` |
| `if/elif/else` | `if/else if/else` ⚠️ |
| `for i in range(10)` | `for (let i = 0; i < 10; i++)` |
| `for item in array` | `for (const item of array)` |
| `match state:` | `switch (state) {` |
| `# comment` | `// comment` |
| `self` | `this` |
| `pass` | `// pass` |

⚠️ = Known limitation, may require manual fixes

## Development

### Project Structure

```
gd2ts/
├── .ai/                   # Strategy and phase documentation
├── addons/
│   ├── gd2ts/            # GDExtension binaries and config
│   └── gut/              # Testing framework
├── src/                   # C++ transpiler core
│   ├── parser/           # Tree-sitter integration
│   ├── transformer/      # AST transformation
│   ├── generator/        # TypeScript generation
│   └── utils/            # Utilities
├── godot-cpp/            # Godot C++ bindings
├── tree-sitter-gdscript/ # tree-sitter-gdscript parser
├── tests/                # Test files and samples
│   ├── unit/             # GUT unit tests
│   ├── gdscript_samples/ # Test GDScript files
│   └── output/           # Transpiled output
└── docs/                 # Documentation
```

### Contributing

Contributions are welcome! Please:

1. Read the strategy document: `.ai/gd2ts-strategy.md`
2. Check existing issues and phase completion summaries
3. Write tests for new features (see `tests/README.md`)
4. Ensure all tests pass before submitting PR
5. Follow C++ and GDScript style conventions

### Development Workflow

```bash
# 1. Make changes to C++ code
scons target=template_debug

# 2. Run tests
godot --headless --path . -s addons/gut/gut_cmdln.gd -gexit

# 3. Test manually
godot --path . -s tests/your_test.gd

# 4. Build release version
scons target=template_release
```

## License

[To be determined]

## Acknowledgments

- [tree-sitter-gdscript](https://github.com/PrestonKnopp/tree-sitter-gdscript) for the GDScript parser
- [godot-cpp](https://github.com/godotengine/godot-cpp) for GDExtension bindings
- Godot Engine team for the amazing game engine
