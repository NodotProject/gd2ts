extends GutTest

# Integration tests for GD2TSConverter using sample files
# These tests validate that real-world GDScript patterns from sample files transpile correctly

var converter: GD2TSConverter
const OUTPUT_DIR = "res://tests/output/"

func before_each():
	# Create a fresh converter instance for each test
	converter = GD2TSConverter.new()
	assert_not_null(converter, "GD2TSConverter should be instantiated")

	# Ensure output directory exists
	if not DirAccess.dir_exists_absolute(OUTPUT_DIR):
		DirAccess.make_dir_absolute(OUTPUT_DIR)

func after_each():
	# Clean up
	converter = null

func _read_file(path: String) -> String:
	if not FileAccess.file_exists(path):
		return ""
	var file = FileAccess.open(path, FileAccess.READ)
	if file:
		var content = file.get_as_text()
		file.close()
		return content
	return ""

# ============================================================================
# Control Flow Sample Tests
# ============================================================================

func test_control_flow_sample():
	var input_path = "res://tests/gdscript_samples/control_flow.gd"
	var output_path = OUTPUT_DIR + "control_flow_test.ts"

	var result = converter.transpile_file(input_path, output_path)

	assert_typeof(result, TYPE_DICTIONARY, "Result should be a dictionary")
	assert_true(result.has("success"), "Result should have success field")

	if result["success"]:
		assert_true(FileAccess.file_exists(output_path), "Output file should be created")

		var content = _read_file(output_path)

		# Validate key features from control_flow.gd
		assert_string_contains(content, "enum", "Should contain enum declaration")
		assert_string_contains(content, "while", "Should contain while loop")
		assert_string_contains(content, "else if", "Should contain else if")
		assert_string_contains(content, "break", "Should contain break statement")
		assert_string_contains(content, "continue", "Should contain continue statement")

		# Clean up
		DirAccess.remove_absolute(output_path)

# ============================================================================
# Signal Sample Tests
# ============================================================================

func test_signals_sample():
	var input_path = "res://tests/gdscript_samples/signals.gd"
	var output_path = OUTPUT_DIR + "signals_test.ts"

	var result = converter.transpile_file(input_path, output_path)

	assert_typeof(result, TYPE_DICTIONARY, "Result should be a dictionary")
	assert_true(result.has("success"), "Result should have success field")

	if result["success"]:
		assert_true(FileAccess.file_exists(output_path), "Output file should be created")

		var content = _read_file(output_path)

		# Validate key features from signals.gd
		assert_string_contains(content, "Signal", "Should contain Signal type")
		assert_string_contains(content, "$", "Signals should have $ prefix")

		# Clean up
		DirAccess.remove_absolute(output_path)

# ============================================================================
# Export Variables Sample Tests
# ============================================================================

func test_export_vars_sample():
	var input_path = "res://tests/gdscript_samples/export_vars.gd"
	var output_path = OUTPUT_DIR + "export_vars_test.ts"

	var result = converter.transpile_file(input_path, output_path)

	assert_typeof(result, TYPE_DICTIONARY, "Result should be a dictionary")
	assert_true(result.has("success"), "Result should have success field")

	if result["success"]:
		assert_true(FileAccess.file_exists(output_path), "Output file should be created")

		var content = _read_file(output_path)

		# Validate key features from export_vars.gd
		assert_string_contains(content, "@exports", "Should contain @exports decorator")
		assert_string_contains(content, "enum", "Should contain enum declaration")

		# Clean up
		DirAccess.remove_absolute(output_path)

# ============================================================================
# Vector Operations Sample Tests
# ============================================================================

func test_vector_operations_sample():
	var input_path = "res://tests/gdscript_samples/vector_operations.gd"
	var output_path = OUTPUT_DIR + "vector_operations_test.ts"

	var result = converter.transpile_file(input_path, output_path)

	assert_typeof(result, TYPE_DICTIONARY, "Result should be a dictionary")
	assert_true(result.has("success"), "Result should have success field")

	if result["success"]:
		assert_true(FileAccess.file_exists(output_path), "Output file should be created")

		var content = _read_file(output_path)

		# Validate key features from vector_operations.gd
		assert_string_contains(content, "Vector2", "Should contain Vector2 type")
		assert_string_contains(content, "Vector3", "Should contain Vector3 type")
		assert_string_contains(content, "Transform2D", "Should contain Transform2D type")
		assert_string_contains(content, "Color", "Should contain Color type")

		# Clean up
		DirAccess.remove_absolute(output_path)

# ============================================================================
# Advanced Features Sample Tests
# ============================================================================

func test_advanced_features_sample():
	var input_path = "res://tests/gdscript_samples/advanced_features.gd"
	var output_path = OUTPUT_DIR + "advanced_features_test.ts"

	var result = converter.transpile_file(input_path, output_path)

	assert_typeof(result, TYPE_DICTIONARY, "Result should be a dictionary")
	assert_true(result.has("success"), "Result should have success field")

	if result["success"]:
		assert_true(FileAccess.file_exists(output_path), "Output file should be created")

		var content = _read_file(output_path)

		# Validate key features from advanced_features.gd
		assert_string_contains(content, "class", "Should contain class declaration")
		assert_string_contains(content, "static", "Should contain static keyword")

		# Clean up
		DirAccess.remove_absolute(output_path)

# ============================================================================
# Godot Specific Sample Tests
# ============================================================================

func test_godot_specific_sample():
	var input_path = "res://tests/gdscript_samples/godot_specific.gd"
	var output_path = OUTPUT_DIR + "godot_specific_test.ts"

	var result = converter.transpile_file(input_path, output_path)

	assert_typeof(result, TYPE_DICTIONARY, "Result should be a dictionary")
	assert_true(result.has("success"), "Result should have success field")

	if result["success"]:
		assert_true(FileAccess.file_exists(output_path), "Output file should be created")

		var content = _read_file(output_path)

		# Validate key features from godot_specific.gd
		assert_string_contains(content, "_ready", "Should contain _ready method")
		assert_string_contains(content, "_process", "Should contain _process method")

		# Clean up
		DirAccess.remove_absolute(output_path)

# ============================================================================
# Simple Class Sample Tests (Already Partially Tested)
# ============================================================================

func test_simple_class_sample():
	var input_path = "res://tests/gdscript_samples/simple_class.gd"
	var output_path = OUTPUT_DIR + "simple_class_test.ts"

	var result = converter.transpile_file(input_path, output_path)

	assert_typeof(result, TYPE_DICTIONARY, "Result should be a dictionary")
	assert_true(result.has("success"), "Result should have success field")

	if result["success"]:
		assert_true(FileAccess.file_exists(output_path), "Output file should be created")

		var content = _read_file(output_path)

		# Validate key features from simple_class.gd
		assert_string_contains(content, "class", "Should contain class declaration")
		assert_string_contains(content, "extends", "Should contain extends keyword")
		assert_string_contains(content, "Signal", "Should contain Signal type")

		# Clean up
		DirAccess.remove_absolute(output_path)
