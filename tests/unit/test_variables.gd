extends GutTest

# Variable declaration tests for GD2TSConverter

var converter: GD2TSConverter

func before_each():
	converter = GD2TSConverter.new()
	assert_not_null(converter, "GD2TSConverter should be instantiated")

func after_each():
	converter = null

# ============================================================================
# Local Variable Tests
# ============================================================================

func test_local_variable_with_type():
	var gdscript = """func test() -> void:
	var items: Array = [1, 2, 3]
	var count: int = 0"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "let", "Local variables should use let")
	assert_string_contains(result, "items", "Variable name should be preserved")
	assert_string_contains(result, "count", "Variable name should be preserved")

func test_local_variable_without_type():
	var gdscript = """func test() -> void:
	var x = 42
	var message = "Hello" """

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "let", "Local variables should use let")
	assert_string_contains(result, "x", "Variable name should be preserved")
	assert_string_contains(result, "message", "Variable name should be preserved")

func test_local_const_variable():
	var gdscript = """func test() -> void:
	const MAX_SPEED: int = 100"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "const", "Const variables should use const")
	assert_string_contains(result, "MAX_SPEED", "Const name should be preserved")
