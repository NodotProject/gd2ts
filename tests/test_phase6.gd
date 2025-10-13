extends SceneTree

# Phase 6 test runner script

func _init():
	print("=== Phase 6 Test Runner ===\n")

	# Create converter instance
	var converter = GD2TSConverter.new()

	if converter == null:
		print("ERROR: Failed to create GD2TSConverter instance")
		quit()
		return

	print("✓ GD2TSConverter instance created successfully\n")

	# Test transpile_file with phase6_test.gd
	print("--- Transpiling phase6_test.gd ---")
	var input_file = "res://tests/phase6_test.gd"
	var output_file = "res://tests/output/phase6_test.ts"

	print("Input:  ", input_file)
	print("Output: ", output_file)

	var result = converter.transpile_file(input_file, output_file)

	if result.get("success", false):
		print("\n✓ Phase 6 transpilation successful!")
		print("  Message: ", result.get("message", ""))

		# Read and display the output
		print("\n--- Generated TypeScript ---\n")
		var file = FileAccess.open(output_file, FileAccess.READ)
		if file:
			var content = file.get_as_text()
			print(content)
			file.close()
		else:
			print("Could not read output file")
	else:
		print("\n✗ Phase 6 transpilation failed!")
		print("  Error: ", result.get("error", "unknown error"))

	print("\n=== Test Complete ===")
	quit()
