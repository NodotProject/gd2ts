@tool
extends Control

# UI Elements
@onready var mode_tabs: TabContainer = $VBoxContainer/ModeTabContainer
@onready var file_path_edit: LineEdit = $VBoxContainer/ModeTabContainer/SingleFile/HBoxContainer/FilePathEdit
@onready var file_browse_button: Button = $VBoxContainer/ModeTabContainer/SingleFile/HBoxContainer/FileBrowseButton
@onready var dir_path_edit: LineEdit = $VBoxContainer/ModeTabContainer/Directory/HBoxContainer/DirPathEdit
@onready var dir_browse_button: Button = $VBoxContainer/ModeTabContainer/Directory/HBoxContainer/DirBrowseButton
@onready var output_path_edit: LineEdit = $VBoxContainer/OutputSettings/HBoxContainer/OutputPathEdit
@onready var output_browse_button: Button = $VBoxContainer/OutputSettings/HBoxContainer/OutputBrowseButton
@onready var transpile_button: Button = $VBoxContainer/TranspileButton
@onready var progress_bar: ProgressBar = $VBoxContainer/ProgressBar
@onready var status_label: Label = $VBoxContainer/StatusLabel
@onready var output_text: TextEdit = $VBoxContainer/OutputText

# Dialogs
var file_dialog: FileDialog
var dir_dialog: FileDialog
var output_dir_dialog: FileDialog
var progress_dialog: AcceptDialog

# GDExtension converter instance
var converter: Object

func _ready() -> void:
	_setup_ui()
	_setup_dialogs()
	_connect_signals()
	_try_load_converter()

func _setup_ui() -> void:
	progress_bar.visible = false
	status_label.text = "Ready"
	output_text.editable = false
	output_text.syntax_highlighter = null

func _setup_dialogs() -> void:
	# File selection dialog for single file mode
	file_dialog = FileDialog.new()
	file_dialog.file_mode = FileDialog.FILE_MODE_OPEN_FILE
	file_dialog.access = FileDialog.ACCESS_RESOURCES
	file_dialog.add_filter("*.gd", "GDScript Files")
	file_dialog.title = "Select GDScript File"
	add_child(file_dialog)

	# Directory selection dialog
	dir_dialog = FileDialog.new()
	dir_dialog.file_mode = FileDialog.FILE_MODE_OPEN_DIR
	dir_dialog.access = FileDialog.ACCESS_RESOURCES
	dir_dialog.title = "Select Directory"
	add_child(dir_dialog)

	# Output directory dialog
	output_dir_dialog = FileDialog.new()
	output_dir_dialog.file_mode = FileDialog.FILE_MODE_OPEN_DIR
	output_dir_dialog.access = FileDialog.ACCESS_RESOURCES
	output_dir_dialog.title = "Select Output Directory"
	add_child(output_dir_dialog)

	# Progress dialog
	progress_dialog = AcceptDialog.new()
	progress_dialog.title = "Transpiling..."
	progress_dialog.dialog_text = "Please wait while files are being transpiled."
	add_child(progress_dialog)

func _connect_signals() -> void:
	if file_browse_button:
		file_browse_button.pressed.connect(_on_file_browse_pressed)
	if dir_browse_button:
		dir_browse_button.pressed.connect(_on_dir_browse_pressed)
	if output_browse_button:
		output_browse_button.pressed.connect(_on_output_browse_pressed)
	if transpile_button:
		transpile_button.pressed.connect(_on_transpile_pressed)

	file_dialog.file_selected.connect(_on_file_selected)
	dir_dialog.dir_selected.connect(_on_dir_selected)
	output_dir_dialog.dir_selected.connect(_on_output_dir_selected)

func _try_load_converter() -> void:
	# Try to load the GDExtension converter
	# This will be implemented once the GDExtension is built
	if ClassDB.class_exists("GD2TSConverter"):
		converter = ClassDB.instantiate("GD2TSConverter")
		_log_message("GD2TS Converter loaded successfully!")
	else:
		_log_error("GD2TS Converter not found. Make sure the GDExtension is compiled and loaded.")
		transpile_button.disabled = true

func _on_file_browse_pressed() -> void:
	file_dialog.popup_centered(Vector2i(800, 600))

func _on_dir_browse_pressed() -> void:
	dir_dialog.popup_centered(Vector2i(800, 600))

func _on_output_browse_pressed() -> void:
	output_dir_dialog.popup_centered(Vector2i(800, 600))

func _on_file_selected(path: String) -> void:
	file_path_edit.text = path

func _on_dir_selected(path: String) -> void:
	dir_path_edit.text = path

func _on_output_dir_selected(path: String) -> void:
	output_path_edit.text = path

func _on_transpile_pressed() -> void:
	if not converter:
		_log_error("Converter not available. Please check that the GDExtension is loaded.")
		return

	var current_tab = mode_tabs.get_current_tab_control()
	var tab_name = mode_tabs.get_tab_title(mode_tabs.current_tab)

	if tab_name == "Single File":
		_transpile_single_file()
	elif tab_name == "Directory":
		_transpile_directory()

func _transpile_single_file() -> void:
	var input_path = file_path_edit.text
	var output_path = output_path_edit.text

	if input_path.is_empty():
		_log_error("Please select a GDScript file to transpile.")
		return

	if output_path.is_empty():
		output_path = input_path.get_base_dir()

	_log_message("Transpiling file: " + input_path)
	_show_progress(true)

	# Construct output file path
	var file_name = input_path.get_file().get_basename() + ".ts"
	var output_file = output_path.path_join(file_name)

	# Call GDExtension transpiler
	var result = converter.transpile_file(input_path, output_file)

	_show_progress(false)

	if result.has("error"):
		_log_error("Transpilation failed: " + str(result.error))
	else:
		_log_message("Successfully transpiled to: " + output_file)

func _transpile_directory() -> void:
	var input_path = dir_path_edit.text
	var output_path = output_path_edit.text

	if input_path.is_empty():
		_log_error("Please select a directory to transpile.")
		return

	if output_path.is_empty():
		output_path = input_path

	_log_message("Transpiling directory: " + input_path)
	_show_progress(true)

	var config = {
		"outDir": output_path,
		"rootDir": input_path
	}

	# Call GDExtension transpiler
	var results = converter.transpile_directory(input_path, config)

	_show_progress(false)

	if results is Array:
		var success_count = 0
		var error_count = 0
		for result in results:
			if result.has("error"):
				error_count += 1
				_log_error("Error in " + str(result.file) + ": " + str(result.error))
			else:
				success_count += 1

		_log_message("Transpilation complete. Success: %d, Errors: %d" % [success_count, error_count])
	else:
		_log_error("Unexpected result from transpiler.")

func _show_progress(visible: bool) -> void:
	progress_bar.visible = visible
	transpile_button.disabled = visible
	if visible:
		status_label.text = "Transpiling..."
	else:
		status_label.text = "Ready"

func _log_message(message: String) -> void:
	output_text.text += "[INFO] " + message + "\n"
	print("GD2TS: " + message)

func _log_error(error: String) -> void:
	output_text.text += "[ERROR] " + error + "\n"
	push_error("GD2TS: " + error)

func show_quick_transpile() -> void:
	# Called from toolbar button for quick transpile
	_log_message("Quick transpile triggered from toolbar")
