extends GutTest

# Class declaration and inheritance tests for GD2TSConverter

var converter: GD2TSConverter

func before_each():
	converter = GD2TSConverter.new()
	assert_not_null(converter, "GD2TSConverter should be instantiated")

func after_each():
	converter = null

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
