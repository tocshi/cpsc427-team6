#pragma once
#include <string>
#include "common.hpp"
#include <array>
#include <map>

enum class Floors {
	FLOOR1 = 0,
	FLOOR_COUNT = FLOOR1 + 1
};
const int floor_count = (int)Floors::FLOOR_COUNT;

class RoomSystem {
public:
	std::string getRandomRoom(Floors floor, bool repeat_allowed);
	Floors current_floor = Floors::FLOOR1;
private:
	const std::map<Floors, std::vector<std::string>> floor_map_data = {
		{
			Floors::FLOOR1, 
			{"map1.tmx", "map2.tmx", "map3.tmx"}
		},
	};

	int current_room_idx = 0;
};