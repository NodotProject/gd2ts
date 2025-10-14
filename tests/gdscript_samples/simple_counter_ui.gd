extends Node
class_name SimpleCounterUI

# Import equivalent - in Godot, UI is built-in
# No need for external imports like ParseUI

var root_widget: Control = null
var counter_text: Label = null
var width: int = 700
var height: int = 100

var bg_color: Color = Color(1, 1, 1)  # RGB
var padding: int = 4

# State management
var is_ui_visible: bool = false
var counter: int = 0

func _init() -> void:
	# Called when you create an object of this UI
	counter = 0

func show() -> void:
	if not root_widget:
		create()
	if not root_widget:
		return

	root_widget.visible = true
	is_ui_visible = true

func hide() -> void:
	if root_widget:
		root_widget.visible = false
		is_ui_visible = false

func create() -> void:
	# Create the root container
	root_widget = PanelContainer.new()
	root_widget.size = Vector2(width, height)
	root_widget.position = Vector2(0, 100)
	root_widget.anchor_top = ANCHOR_BEGIN
	root_widget.anchor_left = ANCHOR_BEGIN

	# Set background color
	var style_box = StyleBoxFlat.new()
	style_box.bg_color = bg_color
	root_widget.add_theme_stylebox_override("panel", style_box)

	# Create inner container with padding
	var inner_container = PanelContainer.new()
	inner_container.size = Vector2(width - padding, height - padding)
	inner_container.position = Vector2(padding / 2, padding / 2)

	var inner_style = StyleBoxFlat.new()
	inner_style.bg_color = Color(0.1, 0.1, 0.1)
	inner_container.add_theme_stylebox_override("panel", inner_style)

	root_widget.add_child(inner_container)

	# Create counter text
	counter_text = Label.new()
	counter_text.text = "Counter: %d" % counter
	counter_text.add_theme_font_size_override("font_size", 36)
	counter_text.size = Vector2(width, 50)
	counter_text.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	counter_text.vertical_alignment = VERTICAL_ALIGNMENT_CENTER

	inner_container.add_child(counter_text)

	# Add to scene tree (assuming this is added to a CanvasLayer or similar)
	# You'd need to add root_widget to your scene

func update() -> void:
	if not counter_text:
		return

	counter_text.text = "Counter: %d" % counter

# Player deployed event equivalent
# In Godot, you'd typically use signals or call this from a player script
func on_player_deployed(event_player: Node) -> void:
	counter = 0
	show()

	# Start the counter loop
	_start_counter_loop()

func _start_counter_loop() -> void:
	while true:
		update()
		await get_tree().create_timer(1.0).timeout  # Wait 1 second
		counter += 1
