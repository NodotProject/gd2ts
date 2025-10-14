extends GutTest

# Tests for specific conversion issues identified in .ai/issues.md
# Tests self/this references, typeof checks, and object literal conversions

var converter: GD2TSConverter

func before_each():
	converter = GD2TSConverter.new()
	assert_not_null(converter, "GD2TSConverter should be instantiated")

func after_each():
	converter = null

# ============================================================================
# Issue 1: self → this conversion
# ============================================================================

func test_self_converted_to_this_in_method_calls():
	var gdscript = """func show() -> void:
	mod.SetUIWidgetVisible(self.rootWidget, true)"""

	var result = converter.transpile_string(gdscript)

	assert_false(result.contains("self."), "self should be converted to this")
	assert_string_contains(result, "this.rootWidget", "Should use this instead of self")

func test_self_converted_to_this_in_assignments():
	var gdscript = """func hide() -> void:
	self.isUIVisible = false"""

	var result = converter.transpile_string(gdscript)

	assert_false(result.contains("self."), "self should be converted to this")
	assert_string_contains(result, "this.isUIVisible", "Should use this instead of self")

func test_self_converted_in_conditional():
	var gdscript = """func create() -> void:
	if not self.rootWidget:
		return"""

	var result = converter.transpile_string(gdscript)

	assert_false(result.contains("self."), "self should be converted to this")
	assert_string_contains(result, "this.rootWidget", "Should use this instead of self")

func test_multiple_self_references_in_one_line():
	var gdscript = """func test() -> void:
	mod.SetUITextLabel(self.counterText, mod.Message(mod.stringkeys.counter, self.counter))"""

	var result = converter.transpile_string(gdscript)

	assert_false(result.contains("self."), "self should be converted to this")
	assert_string_contains(result, "this.counterText", "First self should convert to this")
	assert_string_contains(result, "this.counter", "Second self should convert to this")

func test_self_reference_without_property():
	var gdscript = """func get_self():
	return self"""

	var result = converter.transpile_string(gdscript)

	# 'self' alone (not self.property) should be converted to 'this'
	assert_string_contains(result, "return this", "Standalone self should convert to this")

# ============================================================================
# Issue 2: typeof checks and null comparisons
# ============================================================================

func test_typeof_null_check_converted():
	var gdscript = """func create() -> void:
	if typeof(self.counter) == typeof(null) and self.counter == null:
		return"""

	var result = converter.transpile_string(gdscript)

	# TypeScript should use simpler null checks
	assert_false(result.contains("typeof(null)"), "Should not have typeof(null) in TypeScript")
	assert_false(result.contains("typeof(this.counter)"), "Should not use typeof for null checks")

func test_typeof_comparison_with_type():
	var gdscript = """func test() -> void:
	if typeof(value) == TYPE_INT:
		print("is int")"""

	var result = converter.transpile_string(gdscript)

	# TypeScript uses typeof differently
	assert_false(result.contains("TYPE_INT"), "TYPE_INT constant doesn't exist in TypeScript")

func test_null_check_simplified():
	var gdscript = """func validate() -> void:
	if value == null:
		return"""

	var result = converter.transpile_string(gdscript)

	# null checks should remain simple
	assert_string_contains(result, "null", "null keyword should be preserved")

func test_is_operator_for_type_check():
	var gdscript = """func check_type() -> void:
	if node is Node2D:
		print("Is Node2D")"""

	var result = converter.transpile_string(gdscript)

	# 'is' operator should be converted to TypeScript type checking
	assert_false(result.contains(" is "), "GDScript 'is' should be converted")

# ============================================================================
# Issue 3: Object literal conversion
# ============================================================================

func test_simple_object_literal_preserved():
	var gdscript = """func create() -> void:
	var config = {
		"type": "Container",
		"size": [700, 100],
		"position": [0, 100]
	}"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "type", "Object key should be preserved")
	assert_string_contains(result, "Container", "Object value should be preserved")
	assert_string_contains(result, "size", "Size key should be preserved")
	assert_string_contains(result, "[700, 100]", "Array value should be preserved")

func test_nested_object_literals():
	var gdscript = """func create() -> void:
	var obj = {
		"parent": {
			"child": "value"
		}
	}"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "parent", "Parent key should be preserved")
	assert_string_contains(result, "child", "Child key should be preserved")
	assert_string_contains(result, "value", "Nested value should be preserved")

func test_object_literal_with_array_children():
	var gdscript = """func create() -> void:
	var root = {
		"type": "Container",
		"children": [
			{"type": "Text", "position": [0, 0]},
			{"type": "Button", "size": [100, 50]}
		]
	}"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "children", "children key should be preserved")
	assert_string_contains(result, "Text", "First child type should be preserved")
	assert_string_contains(result, "Button", "Second child type should be preserved")
	# Should NOT have empty object literals like {:, :, :}
	assert_false(result.contains("{:"), "Should not have malformed object literals")

func test_object_literal_with_expressions():
	var gdscript = """func create() -> void:
	var obj = {
		"width": self.width,
		"height": self.height,
		"size": [self.width - self.padding, self.height - self.padding]
	}"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "width", "width key should be preserved")
	assert_string_contains(result, "this.width", "Expression value should use this")
	assert_string_contains(result, "this.padding", "Expression should be preserved")
	assert_false(result.contains("self."), "self should be converted to this")

func test_empty_object_literal():
	var gdscript = """func test() -> void:
	var empty = {}"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "{}", "Empty object should be {}")
	assert_false(result.contains("{:"), "Should not be malformed")

func test_quoted_vs_unquoted_object_keys():
	var gdscript = """func test() -> void:
	var obj1 = {"key": "value"}
	var obj2 = {key: "value"}"""

	var result = converter.transpile_string(gdscript)

	# TypeScript allows both quoted and unquoted keys
	assert_string_contains(result, "key", "Key should be preserved")
	assert_string_contains(result, "value", "Value should be preserved")

# ============================================================================
# Issue 4: Combined issues from SimpleCounterUI example
# ============================================================================

func test_simple_counter_ui_show_method():
	var gdscript = """func show() -> void:
	if not self.rootWidget:
		self.create()

	if not self.rootWidget:
		return

	mod.SetUIWidgetVisible(self.rootWidget, true)
	self.isUIVisible = true"""

	var result = converter.transpile_string(gdscript)

	# Check self conversion
	assert_false(result.contains("self."), "All self references should be converted")
	assert_string_contains(result, "this.rootWidget", "Should use this.rootWidget")
	assert_string_contains(result, "this.create()", "Method call should use this")
	assert_string_contains(result, "this.isUIVisible", "Should use this.isUIVisible")

func test_simple_counter_ui_create_with_typeof():
	var gdscript = """func create() -> void:
	if typeof(self.counter) == typeof(null) and self.counter == null:
		return

	self.rootWidget = ParseUI({"type": "Container"})"""

	var result = converter.transpile_string(gdscript)

	# Check self conversion
	assert_false(result.contains("self."), "All self should be converted to this")

	# Check typeof handling
	assert_false(result.contains("typeof(null)"), "Should not have typeof(null)")

	# Check object literal
	assert_string_contains(result, "type", "Object key should be preserved")
	assert_string_contains(result, "Container", "Object value should be preserved")
