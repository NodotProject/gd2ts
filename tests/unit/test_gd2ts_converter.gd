extends GutTest

# Unit tests for GD2TSConverter
# Tests all major transpilation features

var converter: GD2TSConverter

func before_each():
	# Create a fresh converter instance for each test
	converter = GD2TSConverter.new()
	assert_not_null(converter, "GD2TSConverter should be instantiated")

func after_each():
	# Clean up
	converter = null

# ============================================================================
# Configuration Tests
# ============================================================================

func test_get_version():
	var version = converter.get_version()
	assert_not_null(version, "Version should not be null")
	assert_typeof(version, TYPE_STRING, "Version should be a string")
	assert_gt(version.length(), 0, "Version string should not be empty")

func test_get_default_config():
	var config = converter.get_config()
	assert_not_null(config, "Config should not be null")
	assert_typeof(config, TYPE_DICTIONARY, "Config should be a dictionary")

	# Check for expected default config keys
	assert_true(config.has("inferTypes"), "Config should have inferTypes")
	assert_true(config.has("strictTypes"), "Config should have strictTypes")

func test_set_config():
	var new_config = {
		"inferTypes": false,
		"strictTypes": true,
		"verbose": true
	}

	converter.set_config(new_config)
	var result_config = converter.get_config()

	assert_eq(result_config["inferTypes"], false, "inferTypes should be false")
	assert_eq(result_config["strictTypes"], true, "strictTypes should be true")
	assert_eq(result_config["verbose"], true, "verbose should be true")

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
# Class Declaration Tests
# ============================================================================

func test_transpile_extends():
	var gdscript = "extends Node2D"
	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "extends", "Should contain extends keyword")
	assert_string_contains(result, "Node2D", "Should preserve base class name")

func test_transpile_class_name():
	var gdscript = """extends Node2D
class_name Player"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "class", "Should contain class keyword")
	assert_string_contains(result, "Player", "Should contain class name")
	assert_string_contains(result, "extends", "Should contain extends")
	assert_string_contains(result, "Node2D", "Should contain base class")

func test_transpile_class_with_members():
	var gdscript = """extends Node2D
class_name Player

var health: int = 100

func _ready() -> void:
	print("Ready!")"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "class Player extends Node2D", "Should have proper class declaration")
	assert_string_contains(result, "health", "Should contain class variable")
	assert_string_contains(result, "_ready", "Should contain method")
	assert_string_contains(result, "void", "Should contain return type")

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
# Signal Tests
# ============================================================================

func test_transpile_signal_declaration():
	var gdscript = "signal health_changed(new_health)"
	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "$health_changed", "Signal should have $ prefix")
	assert_string_contains(result, "Signal", "Should use Signal type")

func test_transpile_signal_with_multiple_params():
	var gdscript = "signal player_moved(position, velocity, delta)"
	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "$player_moved", "Signal should have $ prefix")
	assert_string_contains(result, "Signal", "Should use Signal type")

# ============================================================================
# Export Variable Tests
# ============================================================================

func test_transpile_export_variable():
	var gdscript = "export var damage: int = 10"
	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "@exports", "Should have @exports decorator")
	assert_string_contains(result, "damage", "Variable name should be preserved")

func test_transpile_export_with_range():
	var gdscript = "export(int, 0, 100) var health: int = 100"
	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "@exports", "Should have @exports decorator")
	assert_string_contains(result, "health", "Variable name should be preserved")

func test_transpile_export_array():
	var gdscript = "export var items: Array = []"
	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "@exports", "Should have @exports decorator")
	assert_string_contains(result, "items", "Variable name should be preserved")
	assert_string_contains(result, "Array", "Array type should be preserved")

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

# ============================================================================
# @onready Tests
# ============================================================================

func test_transpile_onready_variable():
	var gdscript = "@onready var label: Label = $UI/Label"
	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "label", "Variable name should be preserved")
	assert_string_contains(result, "Label", "Type should be preserved")

func test_transpile_onready_with_get_node():
	var gdscript = "@onready var sprite = get_node(\"Sprite2D\")"
	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "sprite", "Variable name should be preserved")
	assert_string_contains(result, "get_node", "get_node call should be preserved")

# ============================================================================
# Inner Class Tests
# ============================================================================

func test_transpile_inner_class():
	var gdscript = """class InnerClass:
	var value: int = 0

	func increment() -> void:
		value += 1"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "class", "Should contain class keyword")
	assert_string_contains(result, "InnerClass", "Inner class name should be preserved")
	assert_string_contains(result, "increment", "Inner class method should be preserved")

func test_transpile_nested_inner_class():
	var gdscript = """class Outer:
	var x: int = 0

	class Inner:
		var y: int = 0"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "Outer", "Outer class name should be preserved")
	assert_string_contains(result, "Inner", "Inner class name should be preserved")

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
# Complex Integration Tests
# ============================================================================

func test_transpile_complete_class():
	var gdscript = """extends Node2D
class_name Player

# Player health
var health: int = 100
export var max_health: int = 100

signal health_changed(new_health)

func _ready() -> void:
	print("Player ready!")

func take_damage(amount: int) -> void:
	health -= amount
	if health < 0:
		health = 0
	health_changed.emit(health)"""

	var result = converter.transpile_string(gdscript)

	# Check overall structure
	assert_string_contains(result, "class Player extends Node2D", "Should have class declaration")
	assert_string_contains(result, "health", "Should have health variable")
	assert_string_contains(result, "@exports", "Should have @exports decorator")
	assert_string_contains(result, "$health_changed", "Should have signal")
	assert_string_contains(result, "_ready", "Should have _ready method")
	assert_string_contains(result, "take_damage", "Should have take_damage method")

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
