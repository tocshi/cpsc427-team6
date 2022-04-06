#pragma once

#include <vector>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include "world_init.hpp"

// This is a hack to get logText() to work from ai_system
// Source: trust me bro
//class WorldSystem;

class AISystem
{
public:
	void step(Entity e);
	void slime_logic(Entity slime, Entity& player);
	void plant_shooter_logic(Entity plant_shooter, Entity& player);
	void caveling_logic(Entity enemy, Entity& player);
	void king_slime_logic(Entity enemy, Entity& player);
	bool player_in_range(vec2 position, float radius);
};

