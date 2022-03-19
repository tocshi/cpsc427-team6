#include "room_system.hpp"
#include "world_system.hpp"

std::string RoomSystem::getRandomRoom(Floors floor, bool repeat_allowed) {
	const std::vector<std::string>& map_paths = floor_map_data.at(floor);
	if (map_paths.size() == 1) {
		current_room_idx = 0;
		return map_paths[0];
	}
	if (repeat_allowed) {
		current_room_idx = irandRange(0, map_paths.size());
		return map_paths[current_room_idx];
	}
	else {
		// return a random map path not including the currently selected one
		int random_idx = irandRange(0, map_paths.size()-1);
		if (random_idx >= current_room_idx) {
			current_room_idx = random_idx + 1;
		}
		else {
			current_room_idx = random_idx;
		}
		return map_paths[current_room_idx];
	}
}

void RoomSystem::setObjective(ObjectiveType type, int quantity) {
	current_objective = Objective{type, quantity, false};
	switch (type) {
	case ObjectiveType::KILL_ENEMIES:
		world.logText("Kill " + std::to_string(quantity) + " enemies to proceed!");
		break;
	case ObjectiveType::ACTIVATE_SWITCHES:
		world.logText("Find and activate " + std::to_string(quantity) + " switches to proceed!");
		break;
	case ObjectiveType::DESTROY_SPAWNER:
		world.logText("Find and destroy the enemy hive!");
		break;
	case ObjectiveType::SURVIVE_TURNS:
		world.logText("Survive " + std::to_string(quantity) + " turns to proceed!");
		break;
	default:
		break;
	}
	updateObjective(type, 0);
}

void RoomSystem::setRandomObjective() {
	ObjectiveType objective_type = (ObjectiveType)irand(objective_count);
	int quantity = 0;
	switch (objective_type) {
	case ObjectiveType::KILL_ENEMIES:
		quantity = irandRange((int)(registry.enemies.size()*0.5), (int)(registry.enemies.size() * 0.75));
		break;
	case ObjectiveType::ACTIVATE_SWITCHES:
		quantity = irandRange(3, 6);
		break;
	case ObjectiveType::DESTROY_SPAWNER:
		quantity = 1;
		break;
	case ObjectiveType::SURVIVE_TURNS:
		quantity = irandRange(8, 13);
		break;
	default:
		break;
	}
	//setObjective(objective_type, quantity);
	setObjective(ObjectiveType::KILL_ENEMIES, 1);
}

void RoomSystem::updateObjective(ObjectiveType type, int quantity) {
	if (current_objective.type == type) {
		current_objective.remaining_count = max(0, current_objective.remaining_count - quantity);
	}
	if (current_objective.remaining_count == 0) {
		if (!current_objective.completed) {
			current_objective.completed = true;
			world.logText("You hear the sounds of several doors opening in the distance...");
			world.spawn_doors_random_location(3);
			Mix_PlayChannel(-1, world.door_sound, 0);
		}
	}
}