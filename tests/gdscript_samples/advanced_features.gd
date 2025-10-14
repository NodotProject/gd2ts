extends Node
class_name AdvancedFeaturesTest

# Test advanced GDScript features

# Inner classes
class InnerClass:
	var value: int = 0

	func increment() -> void:
		value += 1

	func get_value() -> int:
		return value

# Static functions
static func static_add(a: int, b: int) -> int:
	return a + b

static func static_multiply(a: float, b: float) -> float:
	return a * b

# Const declarations
const PI_VALUE: float = 3.14159
const MAX_PLAYERS: int = 4
const GAME_NAME: String = "MyGame"

# Enum with values
enum Priority {
	LOW = 0,
	MEDIUM = 5,
	HIGH = 10,
	CRITICAL = 100
}

# Array type hints
var int_array: Array[int] = [1, 2, 3, 4, 5]
var string_array: Array[String] = ["one", "two", "three"]

# Dictionary type hints
var player_data: Dictionary = {
	"name": "Player1",
	"level": 5,
	"health": 100
}

# Lambda expressions (anonymous functions)
func test_lambda() -> void:
	var numbers: Array = [1, 2, 3, 4, 5]

	# Filter with lambda
	var evens = numbers.filter(func(n): return n % 2 == 0)

	# Map with lambda
	var doubled = numbers.map(func(n): return n * 2)

	print("Evens: ", evens)
	print("Doubled: ", doubled)

# Await/yield (Godot 4.x)
func async_operation() -> void:
	print("Starting async operation...")
	await get_tree().create_timer(1.0).timeout
	print("Async operation complete!")

# Setget (property getters/setters)
var _private_health: int = 100

var health: int:
	get:
		return _private_health
	set(value):
		_private_health = clamp(value, 0, 100)
		print("Health set to: ", _private_health)

# Type inference with :=
func test_type_inference() -> void:
	var auto_int := 42
	var auto_float := 3.14
	var auto_string := "hello"
	var auto_vector := Vector2(10, 20)

	print("Inferred types work!")

# Ternary operator
func get_status(value: int) -> String:
	return "positive" if value > 0 else "non-positive"

# Multiple return values via array
func get_min_max(values: Array) -> Array:
	if values.is_empty():
		return [0, 0]

	var min_val = values[0]
	var max_val = values[0]

	for val in values:
		if val < min_val:
			min_val = val
		if val > max_val:
			max_val = val

	return [min_val, max_val]

# String formatting
func test_string_formatting() -> void:
	var name: String = "Player"
	var score: int = 1000

	var message1: String = "Hello, %s!" % name
	var message2: String = "%s scored %d points" % [name, score]

	print(message1)
	print(message2)

# Bitwise operations
func test_bitwise() -> void:
	var flags: int = 0

	flags |= 1 << 0  # Set bit 0
	flags |= 1 << 2  # Set bit 2

	var has_flag_0: bool = (flags & (1 << 0)) != 0
	var has_flag_1: bool = (flags & (1 << 1)) != 0

	print("Has flag 0: ", has_flag_0)
	print("Has flag 1: ", has_flag_1)

# Preload and load
const PLAYER_SCENE = preload("res://scenes/player.tscn")

func load_scene() -> void:
	var scene = load("res://scenes/enemy.tscn")
	var instance = scene.instantiate()
	add_child(instance)
