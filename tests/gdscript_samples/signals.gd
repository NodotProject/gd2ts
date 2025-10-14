extends Node
class_name SignalTest

# Test signals with different parameter counts

signal button_clicked
signal health_changed(new_health)
signal player_moved(position, velocity)
signal complex_event(id, name, value, timestamp)

var health: int = 100

func _ready() -> void:
	# Connect signals
	button_clicked.connect(_on_button_clicked)
	health_changed.connect(_on_health_changed)

func _on_button_clicked() -> void:
	print("Button was clicked!")

func _on_health_changed(new_health: int) -> void:
	print("Health changed to: ", new_health)

func trigger_signals() -> void:
	button_clicked.emit()
	health_changed.emit(health)
	player_moved.emit(Vector2(10, 20), Vector2(5, 0))
	complex_event.emit(123, "test", 45.6, Time.get_ticks_msec())
