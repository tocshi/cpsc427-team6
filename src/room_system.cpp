#include "room_system.hpp"

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