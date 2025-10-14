extends GutTest

# Loop statement tests for GD2TSConverter (for, while, break, continue)

var converter: GD2TSConverter

func before_each():
	converter = GD2TSConverter.new()
	assert_not_null(converter, "GD2TSConverter should be instantiated")

func after_each():
	converter = null

# ============================================================================
# For Loop Tests
# ============================================================================

func test_transpile_for_range_simple():
	var gdscript = """func test() -> void:
	for i in range(10):
		print(i)"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "for", "Should contain for keyword")
	assert_string_contains(result, "let i", "Should use let for loop variable")

func test_transpile_for_array():
	var gdscript = """func test() -> void:
	var items = [1, 2, 3]
	for item in items:
		print(item)"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "for", "Should contain for keyword")
	assert_string_contains(result, "of", "Should use 'for...of' syntax")

# ============================================================================
# While Loop Tests
# ============================================================================

func test_transpile_while_loop():
	var gdscript = """func test() -> void:
	var i: int = 0
	while i < 10:
		i += 1"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "while", "Should contain while keyword")
	assert_string_contains(result, "i < 10", "Condition should be preserved")

func test_transpile_while_loop_with_break():
	var gdscript = """func test() -> void:
	var count: int = 0
	while true:
		count += 1
		if count >= 5:
			break"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "while", "Should contain while keyword")
	assert_string_contains(result, "break", "Should contain break statement")

# ============================================================================
# Break/Continue Tests
# ============================================================================

func test_transpile_break_in_for_loop():
	var gdscript = """func test() -> void:
	for i in range(10):
		if i == 5:
			break"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "for", "Should contain for keyword")
	assert_string_contains(result, "break", "Should contain break statement")

func test_transpile_continue_in_for_loop():
	var gdscript = """func test() -> void:
	for i in range(10):
		if i % 2 == 0:
			continue
		print(i)"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "for", "Should contain for keyword")
	assert_string_contains(result, "continue", "Should contain continue statement")

func test_transpile_break_and_continue():
	var gdscript = """func test() -> void:
	for i in range(10):
		if i == 5:
			continue
		if i == 8:
			break"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "continue", "Should contain continue statement")
	assert_string_contains(result, "break", "Should contain break statement")
