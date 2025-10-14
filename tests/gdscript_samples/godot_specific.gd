extends Node2D
class_name GodotSpecificTest

# Test Godot-specific features and patterns

# Node references
@onready var label: Label = $UI/Label
@onready var sprite: Sprite2D = $Sprite2D
@onready var animation_player: AnimationPlayer = $AnimationPlayer

# Signals
signal game_started
signal game_paused(pause_state)
signal player_scored(player_id, points)

# Export groups
@export_group("Player Settings")
@export var player_speed: float = 200.0
@export var player_jump_force: float = 400.0

@export_group("Game Settings")
@export var max_score: int = 1000
@export var time_limit: float = 60.0

# Tool script annotation
@tool

# Node lifecycle methods
func _init() -> void:
	print("Object initialized")

func _ready() -> void:
	print("Node ready")
	game_started.emit()

func _process(delta: float) -> void:
	# Process logic here
	pass

func _physics_process(delta: float) -> void:
	# Physics logic here
	pass

func _input(event: InputEvent) -> void:
	if event is InputEventKey:
		if event.pressed and event.keycode == KEY_ESCAPE:
			get_tree().quit()

func _unhandled_input(event: InputEvent) -> void:
	# Handle unhandled input
	pass

# Node path access
func test_get_node() -> void:
	# Direct $ access
	var my_label = $UI/Label
	var my_sprite = $Sprite2D

	# get_node method
	var node1 = get_node("UI/Label")
	var node2 = get_node("../OtherNode")

	# get_node_or_null for safe access
	var maybe_node = get_node_or_null("MaybeExists")
	if maybe_node:
		print("Node exists!")

	# has_node check
	if has_node("UI/Label"):
		print("Label exists")

# Scene tree operations
func test_scene_tree() -> void:
	# Add and remove children
	var new_node = Node.new()
	new_node.name = "DynamicNode"
	add_child(new_node)

	# Queue free
	new_node.queue_free()

	# Get tree
	var tree = get_tree()
	tree.paused = true

	# Change scene
	tree.change_scene_to_file("res://scenes/main_menu.tscn")

# Input handling
func test_input() -> void:
	# Check input actions
	if Input.is_action_pressed("ui_right"):
		position.x += player_speed * get_process_delta_time()

	if Input.is_action_just_pressed("jump"):
		print("Jump!")

	if Input.is_action_just_released("fire"):
		print("Released fire")

	# Mouse position
	var mouse_pos = get_global_mouse_position()
	print("Mouse at: ", mouse_pos)

# Tween animations
func test_tween() -> void:
	var tween = create_tween()
	tween.tween_property(self, "position", Vector2(100, 100), 1.0)
	tween.tween_property(self, "modulate:a", 0.0, 0.5)

# Timer usage
func test_timer() -> void:
	# Create timer
	var timer = Timer.new()
	timer.wait_time = 2.0
	timer.one_shot = true
	add_child(timer)
	timer.timeout.connect(_on_timer_timeout)
	timer.start()

	# Scene tree timer
	await get_tree().create_timer(1.0).timeout
	print("1 second passed")

func _on_timer_timeout() -> void:
	print("Timer finished!")

# Resource loading
func test_resources() -> void:
	# Load texture
	var texture = load("res://assets/player.png") as Texture2D

	# Preload (compile-time)
	const ICON = preload("res://icon.svg")

	# Instantiate scene
	var scene = load("res://scenes/enemy.tscn") as PackedScene
	var instance = scene.instantiate()
	add_child(instance)

# RPC (Remote Procedure Call) for multiplayer
@rpc("any_peer", "call_local")
func network_update_position(pos: Vector2) -> void:
	position = pos

@rpc("authority", "call_remote", "reliable")
func server_command(command: String) -> void:
	print("Server command: ", command)

# Groups
func test_groups() -> void:
	# Add to group
	add_to_group("enemies")
	add_to_group("ai_controlled")

	# Check group membership
	if is_in_group("enemies"):
		print("This is an enemy")

	# Get nodes in group
	var all_enemies = get_tree().get_nodes_in_group("enemies")
	for enemy in all_enemies:
		print("Found enemy: ", enemy.name)

# Autoload/singleton access
func test_autoload() -> void:
	# Access autoload singleton
	# Assumes "GameManager" is registered as autoload
	# GameManager.start_game()
	# var score = GameManager.get_score()
	pass

# Custom signals and connection
func setup_signals() -> void:
	# Connect to own signals
	game_paused.connect(_on_game_paused)
	player_scored.connect(_on_player_scored)

	# Connect to child node signals
	if has_node("Button"):
		$Button.pressed.connect(_on_button_pressed)

func _on_game_paused(paused: bool) -> void:
	get_tree().paused = paused

func _on_player_scored(player_id: int, points: int) -> void:
	print("Player ", player_id, " scored ", points, " points")

func _on_button_pressed() -> void:
	print("Button pressed!")

# Viewport and camera
func test_viewport() -> void:
	var viewport = get_viewport()
	var viewport_size = viewport.get_visible_rect().size

	var camera = get_viewport().get_camera_2d()
	if camera:
		var camera_pos = camera.get_screen_center_position()
		print("Camera at: ", camera_pos)
