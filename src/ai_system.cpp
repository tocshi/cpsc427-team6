// internal
#include "ai_system.hpp"
#include "combat_system.hpp"
#include "world_system.hpp"

void AISystem::step(Entity e, WorldSystem* world, RenderSystem* renderer)
{
	for (Entity& player : registry.players.entities) {
		if (registry.slimeEnemies.has(e)) {
			slime_logic(e, player, world, renderer);
		}
	}
}

void AISystem::slime_logic(Entity slime, Entity& player, WorldSystem* world, RenderSystem* renderer) {
	Motion& player_motion = registry.motions.get(player);
	Stats& stats = registry.stats.get(slime);
	float chaseRange = stats.range;
	float meleeRange = 100.f;

	Motion& motion_struct = registry.motions.get(slime);

	// Perform melee attack if close enough
	if (registry.slimeEnemies.get(slime).state == ENEMY_STATE::ATTACK) {
		if (player_in_range(motion_struct.position, meleeRange)) {
			createExplosion(renderer, player_motion.position);
			Mix_PlayChannel(-1, world->fire_explosion_sound, 0);
			world->logText(deal_damage(slime, player, 100));
		}
		registry.slimeEnemies.get(slime).state = ENEMY_STATE::AGGRO;
		return;
	}

	// Determine slime state
	// check if player is in range first
	if (player_in_range(motion_struct.position, chaseRange)) {
		registry.slimeEnemies.get(slime).state = ENEMY_STATE::AGGRO;
	}
	else {
		registry.slimeEnemies.get(slime).state = ENEMY_STATE::IDLE;
	}

	ENEMY_STATE state = registry.slimeEnemies.get(slime).state;
	// perform action based on state
	int dx = ichoose(irandRange(-75, -25), irandRange(25, 75));
	int dy = ichoose(irandRange(-75, -25), irandRange(25, 75));

	switch (state) {
	case ENEMY_STATE::IDLE:
		motion_struct.destination = { motion_struct.position.x + dx, motion_struct.position.y + dy };
		motion_struct.velocity = 180.f * normalize(motion_struct.destination - motion_struct.position);
		motion_struct.in_motion = true;
		break;
	case ENEMY_STATE::AGGRO:
		// move towards player
		for (Entity player : registry.players.entities) {
			// get player position
			Motion player_motion = registry.motions.get(player);

			// move towards player
			float dist = distance(motion_struct.position, player_motion.position);
			vec2 direction = normalize(player_motion.position - motion_struct.position);
			float slime_velocity = 180.f;
			float angle = atan2(player_motion.position.y - motion_struct.position.y, player_motion.position.x - motion_struct.position.x);
			float x_component = cos(angle) * slime_velocity;
			float y_component = sin(angle) * slime_velocity;

			motion_struct.velocity = { x_component, y_component };
			motion_struct.destination = motion_struct.position + (direction * 120.f);
			motion_struct.in_motion = true;
		}
		break;
	}
}

// returns true if the player entity is in range of the given position and radius
bool AISystem::player_in_range(vec2 position, float radius) {
	for (Entity player : registry.players.entities) {
		// get player position
		Motion player_motion = registry.motions.get(player);
		float playerX = player_motion.position.x;
		float playerY = player_motion.position.y;

		float xPos = position.x;
		float yPos = position.y;

		// check if position is within the radius of the players position
		double absX = abs(xPos - playerX);
		double absY = abs(yPos - playerY);
		double r = (double)radius;

		if (absX > r || absY > r) {
			return false;
		}
		else if ((absX * absX + absY * absY) <= r * r) {
			return true;
		}
		else {
			return false;
		}
	}
	return false;
}
