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
		world.logText("Defeat " + std::to_string(quantity) + " enemies to proceed!");
		break;
	case ObjectiveType::ACTIVATE_SWITCHES:
		world.spawn_switches_random_location(quantity);
		world.logText("Find and activate " + std::to_string(quantity) + " switches (black tiles) to proceed!");
		break;
	case ObjectiveType::DESTROY_SPAWNER:
		world.logText("Find and destroy the enemy hive!");
		break;
	case ObjectiveType::DEFEAT_BOSS:
		break;
	default:
		break;
	}
	updateObjective(type, 0);
}

void RoomSystem::setRandomObjective() {
	//ObjectiveType objective_type = (ObjectiveType)irand(objective_count);
	ObjectiveType objective_type = (ObjectiveType)irand(2); // For now, only the first 2 are implemented
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
	default:
		break;
	}
	//setObjective(objective_type, quantity);
	setObjective(objective_type, quantity);
}

void RoomSystem::updateObjective(ObjectiveType type, int quantity) {
	if (current_objective.type == type) {
		current_objective.remaining_count = max(0, current_objective.remaining_count - quantity);
	}
	Text& desc = registry.texts.get(world.objectiveDescText);
	Text& remaining = registry.texts.get(world.objectiveNumberText);
	remaining.message = std::to_string(current_objective.remaining_count);
	switch (current_objective.type) {
	case ObjectiveType::KILL_ENEMIES:
		desc.message = "Defeat enemies";
		break;
	case ObjectiveType::ACTIVATE_SWITCHES:
		desc.message = "Activate the black tiles";
		break;
	case ObjectiveType::DESTROY_SPAWNER:
		desc.message = "Destroy the enemy hive";
		break;
	case ObjectiveType::DEFEAT_BOSS:
		desc.message = "Defeat the boss";
		break;
	default:
		desc.message = "";
		remaining.message = "";
	}

	if (current_objective.remaining_count == 0) {
		if (!current_objective.completed) {
			current_objective.completed = true;
			if (rooms_cleared_current_floor >= 1) {
				world.logText("You feel a strong presence coming from a red door...");
				world.spawn_doors_random_location(3, true);
				Mix_PlayChannel(-1, world.door_sound, 0); // TODO: use different sound
			}
			else if (current_floor == Floors::BOSS2) {
				world.logText("A radiant light glimmers before you...");
				Mix_PlayChannel(-1, world.arcane_funnel_sound, 0);
			}
			else if (current_floor == Floors::BOSS1) {
				world.logText("A magicked door appears from the felled enemy...");
				Mix_PlayChannel(-1, world.door_sound, 0);
			}
			else {
				world.logText("You hear the sounds of several doors opening in the distance...");
				world.spawn_doors_random_location(3, false);
				Mix_PlayChannel(-1, world.door_sound, 0);
			}
		}
	}
}

void RoomSystem::updateClearCount() {
	if (!world.tutorial) {
		rooms_cleared_current_floor += 1;
	}
}

void RoomSystem::setNextFloor(Floors floor) {
	current_floor = floor;
	rooms_cleared_current_floor = 0;
}

void RoomSystem::reset() {
	current_floor = Floors::FLOOR1;
	current_room_idx = 0;
	rooms_cleared_current_floor = 0;
}