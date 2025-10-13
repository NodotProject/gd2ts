@tool
extends EditorPlugin

var transpiler_panel: Control
var toolbar_button: Button

func _enter_tree() -> void:
	# Load the transpiler panel scene
	var transpiler_panel_scene = preload("res://addons/gd2ts/ui/transpiler_panel.tscn")
	transpiler_panel = transpiler_panel_scene.instantiate()

	# Add the transpiler panel to the bottom panel
	add_control_to_bottom_panel(transpiler_panel, "GD2TS")

	# Create toolbar button for quick access
	toolbar_button = Button.new()
	toolbar_button.text = "Transpile GDScript"
	toolbar_button.pressed.connect(_on_toolbar_button_pressed)
	add_control_to_container(EditorPlugin.CONTAINER_TOOLBAR, toolbar_button)

	print("GD2TS Plugin: Initialized successfully")

func _exit_tree() -> void:
	# Clean up UI elements
	if transpiler_panel:
		remove_control_from_bottom_panel(transpiler_panel)
		transpiler_panel.queue_free()

	if toolbar_button:
		remove_control_from_container(EditorPlugin.CONTAINER_TOOLBAR, toolbar_button)
		toolbar_button.queue_free()

	print("GD2TS Plugin: Unloaded")

func _on_toolbar_button_pressed() -> void:
	# Show the transpiler panel when toolbar button is pressed
	make_bottom_panel_item_visible(transpiler_panel)

	# Optionally, trigger transpilation directly
	if transpiler_panel.has_method("show_quick_transpile"):
		transpiler_panel.show_quick_transpile()
