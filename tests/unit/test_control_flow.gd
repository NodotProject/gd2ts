extends GutTest

# Control flow and conditional statement tests for GD2TSConverter

var converter: GD2TSConverter

func before_each():
	converter = GD2TSConverter.new()
	assert_not_null(converter, "GD2TSConverter should be instantiated")

func after_each():
	converter = null

# ============================================================================
# Control Flow Tests
# ============================================================================

func test_transpile_if_statement():
	var gdscript = """func test() -> void:
	if health > 0:
		print("Alive")"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "if", "Should contain if keyword")
	assert_string_contains(result, "health > 0", "Condition should be preserved")

func test_transpile_if_else():
	var gdscript = """func test() -> void:
	if health > 0:
		print("Alive")
	else:
		print("Dead")"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "if", "Should contain if")
	assert_string_contains(result, "else", "Should contain else")

func test_transpile_elif():
	var gdscript = """func test() -> void:
	if health > 75:
		print("Healthy")
	elif health > 25:
		print("Wounded")
	else:
		print("Critical")"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "if", "Should contain if")
	assert_string_contains(result, "else if", "elif should convert to else if")

# ============================================================================
# Match Statement Tests
# ============================================================================

func test_transpile_match_statement():
	var gdscript = """func test(state: int) -> void:
	match state:
		0:
			print("Idle")
		1:
			print("Running")
		_:
			print("Unknown")"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "switch", "match should convert to switch")
