// Import Godot types
// import { Signal, Transform2D, Vector2 } from 'godot';

// 
// 
// 
enum State {
	IDLE,
	RUNNING,
	JUMPING,
	FALLING
}
enum WeaponType {
	SWORD = 1,
	BOW = 2,
	STAFF = 3,
	DAGGER = 4
}
// 
position_2d: Vector2 = Vector2(10, 20);
velocity: Vector2 = Vector2(0, 0);
transform_data: Transform2D = Transform2D();
// 
$animation_finished!: Signal;
$movement_complete!: Signal<[any]>;
_ready(): void {
	// 
	test_for_loops();
	// 
	test_vector_operations();
	// 
	test_match_statement();
	// 
	test_enums();
	// 
	test_lambdas();
	// 
	test_async();
	// 
	test_collections();
}

// 
test_for_loops(): void {
	print("=== Testing For Loops ===");
	// 
	for (let i = 0; i < 10; i++) {
		print("Count: ", i);
	}

	// 
	for (let i = 5; i < 15; i++) {
		print("Range: ", i);
	}

	// 
	for (let i = 0; i < 20; i += 2) {
		print("Even: ", i);
	}

	// 
	for (const item of items) {
		print("Item: ", item);
	}

	// 
	for (let i = 0; i < 3; i++) {
		for (let j = 0; j < 3; j++) {
			print("Matrix[", i, "][", j, "] = ", i * 3 + j);
		}

	}

}

// 
test_vector_operations(): void {
	print("=== Testing Vector Operations ===");
	// 
	print("Addition: ", v3);
	// 
	print("Subtraction: ", v_diff);
	// 
	print("Scaled: ", v_scaled, " Product: ", v_product);
	// 
	print("Divided: ", v_divided);
	// 
	print("Chained: ", v_result);
	// 
	print("Normalized: ", normalized, " Rotated: ", rotated);
}

// 
test_match_statement(): void {
	print("=== Testing Match Statements ===");
	// 
	switch (state) {
	}

	// 
	switch (weapon_id) {
	}

	// 
	switch (input) {
	}

}

// 
test_enums(): void {
	print("=== Testing Enums ===");
	print("State: ", current_state);
	print("Weapon: ", weapon);
	// 
	if (current_state == State.IDLE) {
		print("Ready to move");
	}

}

// 
test_lambdas(): void {
	print("=== Testing Lambda Expressions ===");
	// 
	print("Double 5: ", double.call(5));
	// 
	print("Add 3 + 7: ", add.call(3, 7));
	// 
	print("Doubled: ", doubled);
	print("Filtered: ", filtered);
}

// 
test_async(): void {
	print("=== Testing Async/Await ===");
	// 
	start_animation();
	await animation_finished;
	print("Animation finished!");
	// 
	move_to_position(Vector2(100, 100));
	await movement_complete;
	print("Movement complete!");
}

start_animation(): void {
	// 
	await get_tree().create_timer(0.5).timeout;
	animation_finished.emit();
}

move_to_position(target: Vector2): void {
	// 
	await get_tree().create_timer(1.0).timeout;
	;
	movement_complete.emit(target);
}

// 
test_collections(): void {
	print("=== Testing Collections ===");
	// 
	print("Numbers: ", numbers);
	print("Mixed: ", mixed);
	print("Nested: ", nested);
	// 
	print("Player: ", player_data);
	print("Settings: ", complex_dict);
	// 
	numbers.append(6);
	numbers.remove_at(0);
	// 
	;
}

// 
class InnerHelper {
	helper_value: number = 42;
	get_value(): number {
		return helper_value;
	}

	process_data(data: Array<any>): number {
		for (const item of data) {
			;
		}

		return sum;
	}

}

// 
test_inner_class(): void {
	print("Helper value: ", helper.get_value());
	print("Sum: ", result);
}

// 
test_ternary(): void {
	print("Status: ", status);
	print("Rank: ", rank);
}

// 
// 
// 
// 
// 
// 
// 
// 
