extends Node
class_name VectorOperationsTest

# Test vector and transform operations

func test_vector2_operations() -> void:
	var v1: Vector2 = Vector2(10, 20)
	var v2: Vector2 = Vector2(5, 10)

	# Arithmetic operations
	var v_add: Vector2 = v1 + v2
	var v_sub: Vector2 = v1 - v2
	var v_mul: Vector2 = v1 * 2.0
	var v_div: Vector2 = v1 / 2.0

	# Vector with vector operations
	var v_scale: Vector2 = v1 * v2

	print("Addition: ", v_add)
	print("Subtraction: ", v_sub)
	print("Multiplication: ", v_mul)
	print("Division: ", v_div)

func test_vector3_operations() -> void:
	var v1: Vector3 = Vector3(1, 2, 3)
	var v2: Vector3 = Vector3(4, 5, 6)

	var v_add: Vector3 = v1 + v2
	var v_sub: Vector3 = v1 - v2
	var v_mul: Vector3 = v1 * 2.0
	var v_div: Vector3 = v1 / 2.0

	# Dot and cross products
	var dot: float = v1.dot(v2)
	var cross: Vector3 = v1.cross(v2)

	print("Dot product: ", dot)
	print("Cross product: ", cross)

func test_vector_methods() -> void:
	var v: Vector2 = Vector2(3, 4)

	var length: float = v.length()
	var normalized: Vector2 = v.normalized()
	var distance: float = v.distance_to(Vector2(0, 0))
	var angle: float = v.angle()

	print("Length: ", length)
	print("Normalized: ", normalized)
	print("Distance: ", distance)
	print("Angle: ", angle)

func test_transform2d() -> void:
	var transform: Transform2D = Transform2D()

	# Transform operations
	transform = transform.rotated(PI / 4)
	transform = transform.translated(Vector2(10, 20))
	transform = transform.scaled(Vector2(2, 2))

	# Transform a point
	var point: Vector2 = Vector2(5, 5)
	var transformed: Vector2 = transform * point

	print("Transformed point: ", transformed)

func test_color_operations() -> void:
	var c1: Color = Color(1.0, 0.0, 0.0, 1.0)  # Red
	var c2: Color = Color(0.0, 1.0, 0.0, 1.0)  # Green

	var c_blend: Color = c1.blend(c2)
	var c_darkened: Color = c1.darkened(0.5)
	var c_lightened: Color = c1.lightened(0.5)

	print("Blended: ", c_blend)
	print("Darkened: ", c_darkened)
	print("Lightened: ", c_lightened)
