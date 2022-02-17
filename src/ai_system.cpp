// internal
#include "ai_system.hpp"

void AISystem::step(float elapsed_ms)
{

	slime_logic();

	(void)elapsed_ms; // placeholder to silence unused warning until implemented
}

void AISystem::slime_logic() {
	for (Entity& slime : registry.slimeEnemies.entities) {
		SLIME_STATE state = registry.slimeEnemies.get(slime).state;
		float initialY = registry.slimeEnemies.get(slime).initialPosition.y;
		float chaseRange = registry.slimeEnemies.get(slime).chaseRange;
		int dx = ichoose(irandRange(-75, -25), irandRange(25, 75));
		int dy = ichoose(irandRange(-75, -25), irandRange(25, 75));

		Motion& motion_struct = registry.motions.get(slime);

		// perform action based on state, and update state before next turn if required
		switch (state) {
		case SLIME_STATE::IDLE:
			motion_struct.destination = { motion_struct.position.x + dx, motion_struct.position.y + dy };
			motion_struct.velocity = 180.f * normalize(motion_struct.destination - motion_struct.position);
			// check if player is in range first
			if (player_in_range(motion_struct.position, chaseRange)) {
				registry.slimeEnemies.get(slime).state = SLIME_STATE::AGGRO;
			}
			motion_struct.in_motion = true;
			break;
		case SLIME_STATE::AGGRO:
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
				motion_struct.destination = motion_struct.position + (direction * 150.f);
				motion_struct.in_motion = true;
			}

			// if player moves out of range, return to idle animation
			if (!player_in_range(motion_struct.position, chaseRange)) {
				registry.slimeEnemies.get(slime).state = SLIME_STATE::IDLE;
				// need to update initialPosition of slime so it idles from its new location
				registry.slimeEnemies.get(slime).initialPosition = motion_struct.position;
			}
			break;
		}
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
