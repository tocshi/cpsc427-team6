#pragma once

#include <vector>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include "world_init.hpp"

// This is a hack to get logText() to work from ai_system
// Source: trust me bro
//class WorldSystem;

struct AstarNode {
	AstarNode* parent = 0;
	std::vector<AstarNode*> children;
	vec2 position = vec2(0.f, 0.f);
	float g_cost = 0.f;
	float h_cost = 100000000;

	// g_cost + h_cost
	float f_cost= 0;
};

class AISystem
{
bool useSlimeAStar = false;
public:
	void step(Entity e);
	void slime_logic(Entity slime, Entity& player);
	void plant_shooter_logic(Entity plant_shooter, Entity& player);
	void caveling_logic(Entity enemy, Entity& player);
	void king_slime_logic(Entity enemy, Entity& player);
	void living_pebble_logic(Entity enemy, Entity& player);
	void living_rock_logic(Entity enemy, Entity& player);
	void apparition_logic(Entity enemy, Entity& player);
	void reflexion_logic(Entity enemy, Entity& player);
	bool player_in_range(vec2 position, float radius);
};
