extends GutTest

# Basic transpilation tests for GD2TSConverter

var converter: GD2TSConverter

func before_each():
	converter = GD2TSConverter.new()
	assert_not_null(converter, "GD2TSConverter should be instantiated")

func after_each():
	converter = null

# ============================================================================
# Basic Transpilation Tests
# ============================================================================

func test_transpile_empty_string():
	var result = converter.transpile_string("")
	assert_not_null(result, "Result should not be null")
	assert_typeof(result, TYPE_STRING, "Result should be a string")

func test_transpile_simple_variable():
	var gdscript = "var health: int = 100"
	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "health", "Output should contain variable name")
	assert_string_contains(result, "number", "int should be converted to number")
	assert_string_contains(result, "100", "Value should be preserved")

func test_transpile_simple_function():
	var gdscript = """func greet(name: String) -> String:
	return "Hello " + name"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "greet", "Function name should be preserved")
	assert_string_contains(result, "string", "String type should be converted")
	assert_string_contains(result, "return", "Return statement should be preserved")

# ============================================================================
# Comment Preservation Tests
# ============================================================================

func test_preserve_comments():
	var gdscript = """# This is a comment
var health: int = 100  # Inline comment"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "//", "GDScript comments should convert to //")
	assert_string_contains(result, "This is a comment", "Comment text should be preserved")

# ============================================================================
# Import Generation Tests
# ============================================================================

func test_generate_imports():
	var gdscript = """extends Node2D
class_name Test

var position: Vector2 = Vector2(0, 0)"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "import", "Should generate import statement")
	assert_string_contains(result, "godot", "Should import from godot")
	assert_string_contains(result, "Vector2", "Should import Vector2")
	assert_string_contains(result, "Node2D", "Should import Node2D")

# ============================================================================
# File Operations Tests
# ============================================================================

func test_transpile_file():
	var input_path = "res://tests/gdscript_samples/simple_class.gd"
	var output_path = "res://tests/output/test_output.ts"

	var result = converter.transpile_file(input_path, output_path)

	assert_typeof(result, TYPE_DICTIONARY, "Result should be a dictionary")
	assert_true(result.has("success"), "Result should have success field")

	if result["success"]:
		# Check that output file exists
		assert_true(FileAccess.file_exists(output_path), "Output file should be created")

		# Clean up
		DirAccess.remove_absolute(output_path)
