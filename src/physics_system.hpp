#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "components.hpp"
#include "tiny_ecs_registry.hpp"
#include "world_init.hpp"
#include "world_system.hpp"
#include "render_system.hpp"
#include "combat_system.hpp"

// A simple physics system that moves rigid bodies and checks for collision
class PhysicsSystem
{
public:
	void step(float elapsed_ms, WorldSystem* world, RenderSystem* renderer);

	PhysicsSystem()
	{
	}
};

float dist_to(const vec2 position1, const vec2 position2);