// Import Godot types
// import { Signal } from 'godot';

// 
health: number = 100;
speed = 5.0;
@exports
damage: number = 10;
// 
$health_changed!: Signal<[any]>;
const MAX_HEALTH: number = 100;
_ready(): void {
	print("Player ready!");
	health_changed.emit(health);
}

_process(delta: number): void {
	// 
	if (Input.is_action_pressed("ui_right")) {
		;
	}

	if (Input.is_action_pressed("ui_left")) {
		;
	}

	;
}

take_damage(amount: number): void {
	;
	if (health < 0) {
		;
	}

	health_changed.emit(health);
}

heal(amount: number): void {
	;
	if (health > MAX_HEALTH) {
		;
	}

	health_changed.emit(health);
}

get_health_percentage(): number {
	return float(health) / float(MAX_HEALTH) * 100.0;
}

