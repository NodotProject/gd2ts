extends Node2D
class_name Phase6Test

# Phase 6: Advanced Language Features
# This test demonstrates all the advanced features implemented in Phase 6

# Enums
enum State {
	IDLE,
	RUNNING,
	JUMPING,
	FALLING
}

enum WeaponType {
	SWORD = 1,
	BOW = 2,
	STAFF = 3,
	DAGGER = 4
}

# Variables with vector types for operator testing
var position_2d: Vector2 = Vector2(10, 20)
var velocity: Vector2 = Vector2(0, 0)
var transform_data: Transform2D = Transform2D()

# Signals for async testing
signal animation_finished
signal movement_complete(final_position)

func _ready() -> void:
	# Test for loops with range
	test_for_loops()

	# Test vector operations
	test_vector_operations()

	# Test match statements
	test_match_statement()

	# Test enums
	test_enums()

	# Test lambda expressions
	test_lambdas()

	# Test await/yield
	test_async()

	# Test array and dictionary operations
	test_collections()

# For loop tests
func test_for_loops() -> void:
	print("=== Testing For Loops ===")

	# range(stop) - should become: for (let i = 0; i < 10; i++)
	for i in range(10):
		print("Count: ", i)

	# range(start, stop) - should become: for (let i = 5; i < 15; i++)
	for i in range(5, 15):
		print("Range: ", i)

	# range(start, stop, step) - should become: for (let i = 0; i < 20; i += 2)
	for i in range(0, 20, 2):
		print("Even: ", i)

	# Array iteration - should become: for (const item of array)
	var items = [1, 2, 3, 4, 5]
	for item in items:
		print("Item: ", item)

	# Nested for loops
	for i in range(3):
		for j in range(3):
			print("Matrix[", i, "][", j, "] = ", i * 3 + j)

# Vector and transform operator tests
func test_vector_operations() -> void:
	print("=== Testing Vector Operations ===")

	# Addition - should become: v1.add(v2)
	var v1 = Vector2(10, 20)
	var v2 = Vector2(5, 10)
	var v3 = v1 + v2
	print("Addition: ", v3)

	# Subtraction - should become: v1.sub(v2)
	var v_diff = v1 - v2
	print("Subtraction: ", v_diff)

	# Multiplication - should become: v1.mul(scalar) or v1.mul(v2)
	var v_scaled = v1 * 2.0
	var v_product = v1 * v2
	print("Scaled: ", v_scaled, " Product: ", v_product)

	# Division - should become: v1.div(scalar)
	var v_divided = v1 / 2.0
	print("Divided: ", v_divided)

	# Chained operations
	var v_result = (v1 + v2) * 2.0 - Vector2(1, 1)
	print("Chained: ", v_result)

	# Method call operations (should stay as is)
	var normalized = v1.normalized()
	var rotated = v1.rotated(45.0)
	print("Normalized: ", normalized, " Rotated: ", rotated)

# Match statement tests
func test_match_statement() -> void:
	print("=== Testing Match Statements ===")

	var state = State.RUNNING

	# Simple match - should become switch with cases
	match state:
		State.IDLE:
			print("Character is idle")
		State.RUNNING:
			print("Character is running")
		State.JUMPING:
			print("Character is jumping")
		State.FALLING:
			print("Character is falling")
		_:
			print("Unknown state")

	# Match with values
	var weapon_id = 2
	match weapon_id:
		1:
			print("Using sword")
		2:
			print("Using bow")
		3:
			print("Using staff")
		_:
			print("No weapon")

	# Match with multiple patterns (fall-through)
	var input = "a"
	match input:
		"a", "e", "i", "o", "u":
			print("Vowel")
		_:
			print("Consonant")

# Enum tests
func test_enums() -> void:
	print("=== Testing Enums ===")

	var current_state = State.IDLE
	print("State: ", current_state)

	var weapon = WeaponType.SWORD
	print("Weapon: ", weapon)

	# Enum in condition
	if current_state == State.IDLE:
		print("Ready to move")

# Lambda expression tests (GDScript 2.0)
func test_lambdas() -> void:
	print("=== Testing Lambda Expressions ===")

	# Simple lambda - should become: (x) => x * 2
	var double = func(x): return x * 2
	print("Double 5: ", double.call(5))

	# Lambda with multiple parameters - should become: (x, y) => x + y
	var add = func(x, y): return x + y
	print("Add 3 + 7: ", add.call(3, 7))

	# Lambda used in array operations
	var numbers = [1, 2, 3, 4, 5]
	var doubled = numbers.map(func(x): return x * 2)
	print("Doubled: ", doubled)

	var filtered = numbers.filter(func(x): return x > 2)
	print("Filtered: ", filtered)

# Async/await tests
func test_async() -> void:
	print("=== Testing Async/Await ===")

	# Await signal - should become: yield this.$animation_finished
	start_animation()
	await animation_finished
	print("Animation finished!")

	# Await with parameters
	move_to_position(Vector2(100, 100))
	await movement_complete
	print("Movement complete!")

func start_animation() -> void:
	# Simulate animation
	await get_tree().create_timer(0.5).timeout
	animation_finished.emit()

func move_to_position(target: Vector2) -> void:
	# Simulate movement
	await get_tree().create_timer(1.0).timeout
	position_2d = target
	movement_complete.emit(target)

# Array and Dictionary tests
func test_collections() -> void:
	print("=== Testing Collections ===")

	# Array literals
	var numbers = [1, 2, 3, 4, 5]
	var mixed = [1, "hello", true, Vector2(0, 0)]
	var nested = [[1, 2], [3, 4], [5, 6]]

	print("Numbers: ", numbers)
	print("Mixed: ", mixed)
	print("Nested: ", nested)

	# Dictionary literals
	var player_data = {
		"name": "Player1",
		"health": 100,
		"position": Vector2(0, 0)
	}

	var complex_dict = {
		"settings": {
			"volume": 0.8,
			"fullscreen": true
		},
		"scores": [100, 200, 300]
	}

	print("Player: ", player_data)
	print("Settings: ", complex_dict)

	# Array operations
	numbers.append(6)
	numbers.remove_at(0)
	var first = numbers[0]

	# Dictionary operations
	player_data["score"] = 1000
	var player_name = player_data["name"]

# Inner class (nested class) test
class InnerHelper:
	var helper_value: int = 42

	func get_value() -> int:
		return helper_value

	func process_data(data: Array) -> int:
		var sum = 0
		for item in data:
			sum += item
		return sum

# Using inner class
func test_inner_class() -> void:
	var helper = InnerHelper.new()
	print("Helper value: ", helper.get_value())

	var data = [1, 2, 3, 4, 5]
	var result = helper.process_data(data)
	print("Sum: ", result)

# Conditional expression (ternary) test
func test_ternary() -> void:
	var health = 50
	var status = "alive" if health > 0 else "dead"
	print("Status: ", status)

	var level = 10
	var rank = "beginner" if level < 5 else "intermediate" if level < 15 else "expert"
	print("Rank: ", rank)

# Property with setget (to be implemented)
# var health: int = 100 setget set_health, get_health
#
# func set_health(value: int) -> void:
#     health = max(0, value)
#
# func get_health() -> int:
#     return health
