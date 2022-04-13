#pragma once
#include <string>
#include "common.hpp"
#include <array>
#include <map>
#include "tiny_ecs_registry.hpp"

class RoomSystem {
public:
	std::string getRandomRoom(Floors floor, bool repeat_allowed);
	void updateObjective(ObjectiveType type, int quantity);
	void setObjective(ObjectiveType type, int quantity);
	void setRandomObjective();
	void updateClearCount();
	void setNextFloor(Floors floor);
	void reset();

	Floors current_floor = Floors::FLOOR1;
	int current_room_idx = 0;
	int rooms_cleared_current_floor = 0;
	Objective current_objective;
private:
	const std::map<Floors, std::vector<std::string>> floor_map_data = {
		{
			Floors::DEBUG,
			{"debug_room.tmx"}
		},
		{
			Floors::TUTORIAL,
			{"tutorial1.tmx"}
		},
		{
			Floors::FLOOR1, 
			{"map1.tmx", "map2.tmx", "map3.tmx"}
		},
		{
			Floors::BOSS1,
			{"boss1.tmx"}
		},
		{
			Floors::FLOOR2,
			{"map4.tmx", "map5.tmx", "map6.tmx"}
		},
		{
			Floors::BOSS2,
			{"boss2.tmx"}
		}
	};
};