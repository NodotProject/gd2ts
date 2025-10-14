extends GutTest

# Type system and conversion tests for GD2TSConverter

var converter: GD2TSConverter

func before_each():
	converter = GD2TSConverter.new()
	assert_not_null(converter, "GD2TSConverter should be instantiated")

func after_each():
	converter = null

# ============================================================================
# Type System Tests
# ============================================================================

func test_type_int_to_number():
	var gdscript = "var count: int = 42"
	var result = converter.transpile_string(gdscript)
	assert_string_contains(result, "number", "int should convert to number")

func test_type_float_to_number():
	var gdscript = "var speed: float = 5.5"
	var result = converter.transpile_string(gdscript)
	assert_string_contains(result, "number", "float should convert to number")

func test_type_string():
	var gdscript = 'var name: String = "Player"'
	var result = converter.transpile_string(gdscript)
	assert_string_contains(result, "string", "String should convert to string")

func test_type_bool():
	var gdscript = "var is_alive: bool = true"
	var result = converter.transpile_string(gdscript)
	assert_string_contains(result, "boolean", "bool should convert to boolean")

func test_type_vector2():
	var gdscript = "var position: Vector2 = Vector2(10, 20)"
	var result = converter.transpile_string(gdscript)
	assert_string_contains(result, "Vector2", "Vector2 type should be preserved")

# ============================================================================
# Enum Tests
# ============================================================================

func test_transpile_enum():
	var gdscript = """enum State {
	IDLE,
	RUNNING,
	JUMPING
}"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "enum", "Should contain enum keyword")
	assert_string_contains(result, "State", "Should preserve enum name")
	assert_string_contains(result, "IDLE", "Should contain enum values")

# ============================================================================
# Vector Operations Tests
# ============================================================================

func test_transpile_vector_addition():
	var gdscript = """func test() -> void:
	var v1 = Vector2(1, 2)
	var v2 = Vector2(3, 4)
	var v3 = v1 + v2"""

	var result = converter.transpile_string(gdscript)

	# Vector operations should be converted to method calls
	assert_string_contains(result, "Vector2", "Should contain Vector2")
