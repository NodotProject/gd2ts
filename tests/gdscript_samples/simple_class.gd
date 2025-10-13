extends Node2D
class_name Player

# A simple player class for testing transpilation

var health: int = 100
var speed: float = 5.0
export var damage: int = 10

signal health_changed(new_health)

func _ready():
	print("Player ready!")
	health_changed.emit(health)

func take_damage(amount: int) -> void:
	health -= amount
	if health < 0:
		health = 0
	health_changed.emit(health)

func move(delta: float) -> void:
	var velocity = Vector2(speed, 0)
	position += velocity * delta
