extends Node

# Simple test script to verify GD2TSConverter works

func _ready():
	print("=== GD2TS Test ===")

	# Create converter instance
	var converter = GD2TSConverter.new()

	if converter == null:
		print("ERROR: Failed to create GD2TSConverter instance")
		return

	print("✓ GD2TSConverter instance created successfully")

	# Test get_version
	var version = converter.get_version()
	print("✓ Version: ", version)

	# Test get_config
	var config = converter.get_config()
	print("✓ Default config: ", config)

	# Test set_config
	converter.set_config({
		"inferTypes": false,
		"strictTypes": true,
		"verbose": true
	})
	print("✓ Config updated")

	var new_config = converter.get_config()
	print("  New config: ", new_config)

	# Test transpile_string
	var test_gdscript = """
extends Node2D

var health: int = 100

func _ready():
	print("Hello from GDScript!")
"""

	print("\n--- Testing transpile_string ---")
	print("Input GDScript:")
	print(test_gdscript)

	var result = converter.transpile_string(test_gdscript)
	print("\nOutput TypeScript:")
	print(result)

	# Test transpile_file with simple_class.gd
	print("\n--- Testing transpile_file with simple_class.gd ---")
	var input_file = "res://tests/gdscript_samples/simple_class.gd"
	var output_file = "res://tests/output/simple_class.ts"
	print("Input:  ", input_file)
	print("Output: ", output_file)

	var file_result = converter.transpile_file(input_file, output_file)
	print("Result: ", file_result)

	if file_result.get("success", false):
		print("✓ File transpilation successful!")
		print("  Message: ", file_result.get("message", ""))
	else:
		print("✗ File transpilation failed!")
		print("  Error: ", file_result.get("error", "unknown error"))

	print("\n=== Test Complete ===")

	# Quit after tests
	get_tree().quit()
