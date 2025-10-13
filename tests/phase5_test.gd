@tool
extends Node2D
class_name Phase5Test

# Signal declarations
signal health_changed(new_health)
signal player_died

# Export variables
@export var max_health: int = 100
@export var speed: float = 250.0

# Regular variables
var current_health: int = 100
@onready var player_sprite = $PlayerSprite

# Constant
const GRAVITY: float = 980.0

# Preload/load
const PlayerScene = preload("res://player.tscn")
var loaded_scene = load("res://enemy.tscn")

func _ready() -> void:
	# Get node with $ syntax
	var ui = $UI/HealthBar
	var label = get_node("UI/Label")

	# Signal connections
	health_changed.connect(_on_health_changed)
	player_died.connect(_on_player_died)

	# Signal emission
	health_changed.emit(current_health)

	print("Ready!")

func take_damage(amount: int) -> void:
	current_health -= amount
	health_changed.emit(current_health)

	if current_health <= 0:
		player_died.emit()

func _on_health_changed(new_health: int) -> void:
	print("Health changed to: ", new_health)

func _on_player_died() -> void:
	print("Player died!")
	queue_free()

# Autoload reference example
func save_game() -> void:
	# Assuming GameManager is an autoload
	GameManager.save_player_data()
