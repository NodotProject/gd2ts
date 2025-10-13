extends Node2D
class_name Player

# Player health variable
var health: int = 100
var speed := 5.0
export var damage: int = 10

# Signal when health changes
signal health_changed(new_health)

const MAX_HEALTH: int = 100

func _ready() -> void:
	print("Player ready!")
	health_changed.emit(health)

func _process(delta: float) -> void:
	# Move the player
	var velocity = Vector2(0, 0)
	
	if Input.is_action_pressed("ui_right"):
		velocity.x += speed
	if Input.is_action_pressed("ui_left"):
		velocity.x -= speed
	
	position += velocity * delta

func take_damage(amount: int) -> void:
	health -= amount
	if health < 0:
		health = 0
	health_changed.emit(health)

func heal(amount: int) -> void:
	health += amount
	if health > MAX_HEALTH:
		health = MAX_HEALTH
	health_changed.emit(health)

func get_health_percentage() -> float:
	return float(health) / float(MAX_HEALTH) * 100.0
