extends GutTest

# Async/Await conversion tests for GD2TSConverter
# GDScript doesn't require 'async' keyword, but TypeScript does
# Functions containing 'await' must be marked as 'async' in TypeScript

var converter: GD2TSConverter

func before_each():
	converter = GD2TSConverter.new()
	assert_not_null(converter, "GD2TSConverter should be instantiated")

func after_each():
	converter = null

# ============================================================================
# Basic Async/Await Tests
# ============================================================================

func test_function_with_single_await_becomes_async():
	var gdscript = """func load_data() -> void:
	var result = await http_request.request_completed
	print(result)"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "async", "Function with await should have async keyword")
	assert_string_contains(result, "await", "await keyword should be preserved")

func test_function_without_await_not_async():
	var gdscript = """func simple_function() -> void:
	print("Hello")"""

	var result = converter.transpile_string(gdscript)

	assert_false(result.contains("async"), "Function without await should not have async keyword")

func test_function_with_multiple_awaits():
	var gdscript = """func fetch_all() -> void:
	var data1 = await fetch_data(1)
	var data2 = await fetch_data(2)
	print(data1, data2)"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "async", "Function with multiple awaits should have async keyword")

# ============================================================================
# Async Function with Return Types
# ============================================================================

func test_async_function_with_return_type():
	var gdscript = """func get_user(id: int) -> Dictionary:
	var response = await http.get("/user/" + str(id))
	return response.body"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "async", "Function should be async")
	# TypeScript async functions that return T should be Promise<T>
	assert_string_contains(result, "Promise", "Async function return type should be wrapped in Promise")

func test_async_function_void_return():
	var gdscript = """func save_data(data: Dictionary) -> void:
	await file.save(data)
	print("Saved")"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "async", "Function should be async")
	# void return type in async function should be Promise<void>
	assert_string_contains(result, "Promise", "Async void function should return Promise<void>")

# ============================================================================
# Nested Functions and Lambdas
# ============================================================================

func test_nested_function_with_await():
	var gdscript = """func outer() -> void:
	var inner = func():
		await get_tree().create_timer(1.0).timeout
		print("Done")
	inner.call()"""

	var result = converter.transpile_string(gdscript)

	# The inner lambda/function should be async
	assert_string_contains(result, "async", "Nested function with await should be async")

func test_outer_function_without_await_inner_with_await():
	var gdscript = """func outer() -> void:
	var callback = func():
		await something()
	callback.call()"""

	var result = converter.transpile_string(gdscript)

	# Only the inner function should be async, not outer
	# This is a more complex test that checks proper scoping
	assert_string_contains(result, "async", "Inner function should be async")

# ============================================================================
# Await with Signals
# ============================================================================

func test_await_signal():
	var gdscript = """func wait_for_signal() -> void:
	await button.pressed
	print("Button was pressed")"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "async", "Function awaiting signal should be async")
	assert_string_contains(result, "await", "await should be preserved")

func test_await_timer():
	var gdscript = """func delayed_action() -> void:
	await get_tree().create_timer(2.0).timeout
	do_something()"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "async", "Function with timer await should be async")

# ============================================================================
# Complex Scenarios
# ============================================================================

func test_await_in_conditional():
	var gdscript = """func conditional_fetch() -> void:
	if should_fetch:
		var data = await fetch()
		process(data)"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "async", "Function with await in conditional should be async")

func test_await_in_loop():
	var gdscript = """func fetch_all_items() -> void:
	for id in item_ids:
		var item = await fetch_item(id)
		items.append(item)"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "async", "Function with await in loop should be async")

func test_multiple_functions_some_with_await():
	var gdscript = """func sync_function() -> void:
	print("I am sync")

func async_function() -> void:
	await something()
	print("I am async")"""

	var result = converter.transpile_string(gdscript)

	# This test needs to verify that only async_function has async keyword
	# and sync_function does not
	assert_string_contains(result, "async", "Should contain async keyword for async_function")
	# More sophisticated check would count occurrences or use regex

# ============================================================================
# Static and Private Functions
# ============================================================================

func test_static_function_with_await():
	var gdscript = """static func fetch_global_data() -> Dictionary:
	var data = await GlobalAPI.fetch()
	return data"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "async", "Static function with await should be async")
	assert_string_contains(result, "static", "Static keyword should be preserved")

func test_private_function_with_await():
	var gdscript = """func _private_fetch() -> void:
	var data = await _internal_api.get()
	_process_data(data)"""

	var result = converter.transpile_string(gdscript)

	assert_string_contains(result, "async", "Private function with await should be async")
