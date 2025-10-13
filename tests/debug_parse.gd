extends Node

func _ready():
	print("=== Debug Parse Test ===")

	var converter = GD2TSConverter.new()

	# Simple test
	var test_code = "var x: int = 42"

	print("\n--- Input ---")
	print(test_code)

	var result = converter.transpile_string(test_code)

	print("\n--- Output ---")
	print(result)

	print("\n--- Test 2: Simple class ---")
	var test2 = """extends Node
var health: int = 100
"""

	print("Input:")
	print(test2)

	var result2 = converter.transpile_string(test2)
	print("\nOutput:")
	print(result2)

	get_tree().quit()
