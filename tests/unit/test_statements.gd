extends GutTest

# Statement tests for GD2TSConverter (assignments, method calls, constructors)

var converter: GD2TSConverter

func before_each():
	converter = GD2TSConverter.new()
	assert_not_null(converter, "GD2TSConverter should be instantiated")

func after_each():
	converter = null

# ============================================================================
# Assignment Statement Tests
# ============================================================================

func test_simple_assignment():
	var gdscript = """func test() -> void:
	var x: int = 5
	x = 10"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "x = 10", "Should contain assignment statement")

func test_attribute_assignment():
	var gdscript = """func test() -> void:
	root_widget.visible = true"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "root_widget.visible = true", "Should contain attribute assignment")

func test_multiple_assignments():
	var gdscript = """func test() -> void:
	var x: int = 0
	var y: int = 0
	x = 5
	y = 10
	x = y"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "x = 5", "Should contain first assignment")
	assert_string_contains(result, "y = 10", "Should contain second assignment")
	assert_string_contains(result, "x = y", "Should contain third assignment")

func test_assignment_with_expression():
	var gdscript = """func test() -> void:
	var counter: int = 0
	counter = counter + 1"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "counter = counter + 1", "Should contain assignment with expression")

# ============================================================================
# Method Call Statement Tests
# ============================================================================

func test_simple_method_call():
	var gdscript = """func test() -> void:
	print("Hello")"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, 'print("Hello")', "Should contain method call")

func test_method_call_on_object():
	var gdscript = """func test() -> void:
	root_widget.show()"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "root_widget.show()", "Should contain method call on object")

func test_chained_method_calls():
	var gdscript = """func test() -> void:
	get_tree().create_timer(1.0).timeout"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "get_tree().create_timer(1.0).timeout", "Should contain chained method calls")

func test_method_call_with_multiple_args():
	var gdscript = """func test() -> void:
	root_widget.add_theme_stylebox_override("panel", style_box)"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "add_theme_stylebox_override", "Should contain method name")
	assert_string_contains(result, '"panel"', "Should contain first argument")
	assert_string_contains(result, "style_box", "Should contain second argument")

# ============================================================================
# Constructor Call Tests
# ============================================================================

func test_constructor_call_assignment():
	var gdscript = """func test() -> void:
	var widget = PanelContainer.new()"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "PanelContainer.new()", "Should contain constructor call")

func test_constructor_with_params():
	var gdscript = """func test() -> void:
	var vec = Vector2(10, 20)"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "Vector2(10, 20)", "Should contain constructor with parameters")

# ============================================================================
# Real-World Pattern Tests
# ============================================================================

func test_ui_creation_pattern():
	var gdscript = """func create() -> void:
	root_widget = PanelContainer.new()
	root_widget.size = Vector2(700, 100)
	root_widget.visible = true"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "root_widget = PanelContainer.new()", "Should have constructor assignment")
	assert_string_contains(result, "root_widget.size = Vector2(700, 100)", "Should have property assignment")
	assert_string_contains(result, "root_widget.visible = true", "Should have boolean assignment")

func test_counter_loop_pattern():
	var gdscript = """func update_loop() -> void:
	var counter: int = 0
	while true:
		update()
		await get_tree().create_timer(1.0).timeout
		counter += 1"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "counter += 1", "Should contain augmented assignment")
	assert_string_contains(result, "update()", "Should contain method call")
	assert_string_contains(result, "await", "Should contain await keyword")
