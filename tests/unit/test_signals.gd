extends GutTest

# Signal declaration and handling tests for GD2TSConverter

var converter: GD2TSConverter

func before_each():
	converter = GD2TSConverter.new()
	assert_not_null(converter, "GD2TSConverter should be instantiated")

func after_each():
	converter = null

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
