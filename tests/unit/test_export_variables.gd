extends GutTest

# Export variable and @onready decorator tests for GD2TSConverter

var converter: GD2TSConverter

func before_each():
	converter = GD2TSConverter.new()
	assert_not_null(converter, "GD2TSConverter should be instantiated")

func after_each():
	converter = null

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
