#pragma once

#include <vector>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include "world_init.hpp"

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// DON'T WORRY ABOUT THIS CLASS UNTIL ASSIGNMENT 3
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// This is a hack to get logText() to work from ai_system
// Source: trust me bro
class WorldSystem;

class AISystem
{
public:
	void step(Entity e, WorldSystem* world, RenderSystem* renderer);
	void slime_logic(Entity slime, Entity& player, WorldSystem* world, RenderSystem* renderer);
	bool player_in_range(vec2 position, float radius);
};

