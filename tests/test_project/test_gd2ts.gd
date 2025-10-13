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

	# Test transpile_file
	print("\n--- Testing transpile_file ---")
	var file_result = converter.transpile_file("res://test_input.gd", "res://test_output.ts")
	print("Result: ", file_result)

	print("\n=== Test Complete ===")
