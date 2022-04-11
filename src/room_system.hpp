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

	Floors current_floor = Floors::FLOOR2; // todo: change back to floor1
	int current_room_idx = 0;
	int rooms_cleared_current_floor = 0;
	Objective current_objective;
private:
	const std::map<Floors, std::vector<std::string>> floor_map_data = {
		{
			Floors::TUTORIAL,
			{"tutorial1.tmx"}
		},
		{
			Floors::DEBUG,
			{"debug_room.tmx"}
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
			{"map1.tmx", "map2.tmx", "map3.tmx"}	// TODO: replace with floor 2 maps
		}
	};
};