extends Node
class_name ControlFlowTest

# Test all control flow structures

enum State {
	IDLE,
	RUNNING,
	JUMPING,
	FALLING
}

var current_state: State = State.IDLE

func test_if_elif_else(value: int) -> String:
	if value > 100:
		return "high"
	elif value > 50:
		return "medium"
	elif value > 0:
		return "low"
	else:
		return "zero or negative"

func test_nested_if(x: int, y: int) -> bool:
	if x > 0:
		if y > 0:
			return true
		else:
			return false
	else:
		return false

func test_while_loop() -> int:
	var count: int = 0
	while count < 10:
		count += 1
	return count

func test_for_range() -> void:
	# Simple range
	for i in range(10):
		print(i)

	# Range with start and stop
	for i in range(5, 15):
		print(i)

	# Range with start, stop, and step
	for i in range(0, 100, 10):
		print(i)

func test_for_array() -> void:
	var items: Array = [1, 2, 3, 4, 5]
	for item in items:
		print(item)

func test_match_statement(state: State) -> String:
	match state:
		State.IDLE:
			return "Character is idle"
		State.RUNNING:
			return "Character is running"
		State.JUMPING:
			return "Character is jumping"
		State.FALLING:
			return "Character is falling"
		_:
			return "Unknown state"

func test_match_with_values(value: int) -> String:
	match value:
		0:
			return "zero"
		1, 2, 3:
			return "small"
		_:
			return "other"

func test_break_continue() -> Array:
	var results: Array = []

	for i in range(20):
		if i == 5:
			continue
		if i == 15:
			break
		results.append(i)

	return results
