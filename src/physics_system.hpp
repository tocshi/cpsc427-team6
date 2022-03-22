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
private:
	float knock_base_velocity = 500.f;
	float knock_min_velocity = 10.f;
	float knock_decel_threshold = 64.f;
	
};

float dist_to(const vec2 position1, const vec2 position2);

bool collides_AABB(const Motion& motion1, const Motion& motion2);
bool collides_circle(const Motion& motion1, const Motion& motion2);
bool testRectangleToPoint(float rectWidth, float rectHeight, float rectRotation, float rectCenterX, float rectCenterY, float pointX, float pointY);
bool testCircleToSegment(float circleCenterX, float circleCenterY, float circleRadius, float lineAX, float lineAY, float lineBX, float lineBY);
bool collides_rotrect_circle(const Motion& rectmotion, const Motion& circlemotion);