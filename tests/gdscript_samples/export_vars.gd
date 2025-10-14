extends Node2D
class_name ExportVariablesTest

# Test various export variable patterns

export var basic_export: int = 10
export var string_export: String = "default"
export var float_export: float = 3.14

# Export with range hints (these become decorators in TypeScript)
export(int, 0, 100) var health: int = 100
export(float, 0.0, 1.0) var opacity: float = 1.0

# Export arrays
export var item_list: Array = []
export var number_list: Array = [1, 2, 3]

# Export node paths
export var target_node: NodePath
export var animation_player: NodePath

# Export resources
export var texture: Texture2D
export var material: Material

# Export enums
enum Element {
	FIRE,
	WATER,
	EARTH,
	AIR
}

export(Element) var element_type: Element = Element.FIRE

# Export flags
export(int, FLAGS, "Red", "Green", "Blue") var color_flags: int = 0

func _ready() -> void:
	print("Export variables test ready")
	print("Health: ", health)
	print("Opacity: ", opacity)
	print("Element: ", element_type)
