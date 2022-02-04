// internal
#include "ai_system.hpp"

void AISystem::step(float elapsed_ms)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A2: HANDLE BUG AI HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
	// You will likely want to write new functions and need to create
	// new data structures to implement a more sophisticated Bug AI.
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	slime_logic();

	(void)elapsed_ms; // placeholder to silence unused warning until implemented

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A2: DRAW DEBUG INFO HERE on AI path
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
	// You will want to use the createLine from world_init.hpp
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}

void AISystem::slime_logic() {
	for (Entity& slime : registry.slimeEnemies.entities) {
		SLIME_STATE state = registry.slimeEnemies.get(slime).state;
		float initialY = registry.slimeEnemies.get(slime).initialPosition.y;
		float chaseRange = registry.slimeEnemies.get(slime).chaseRange;

		Motion& motion_struct = registry.motions.get(slime);

		// perform action based on state, and update state before next turn if required
		switch (state) {
			case SLIME_STATE::IDLE_DOWN: 
				motion_struct.destination = { motion_struct.position.x, motion_struct.position.y + 75 };
				motion_struct.velocity = { 0, 150 };
				// check if player is in range first
				if (player_in_range(motion_struct.position, chaseRange)) {
					registry.slimeEnemies.get(slime).state = SLIME_STATE::CHASING;
				} else if (motion_struct.position.y > initialY + 150) {
					// if the idle boundry is hit, bounce up
					registry.slimeEnemies.get(slime).state = SLIME_STATE::IDLE_UP;
				}
				motion_struct.in_motion = true;
				break;
			case SLIME_STATE::IDLE_UP: 
				motion_struct.destination = { motion_struct.position.x, motion_struct.position.y - 75 };
				motion_struct.velocity = { 0, -150 };

				// check if player is in range first
				if (player_in_range(motion_struct.position, chaseRange)) {
					registry.slimeEnemies.get(slime).state = SLIME_STATE::CHASING;
				} else if (motion_struct.position.y < initialY - 150) {
					// if the idle boundry is hit, bounce down
					registry.slimeEnemies.get(slime).state = SLIME_STATE::IDLE_DOWN;
				}
				motion_struct.in_motion = true;
				break;
			case SLIME_STATE::CHASING:
				// move towards player
				for (Entity player : registry.players.entities) {
					// get player position
					Motion player_motion = registry.motions.get(player);

					// move towards player
					float dist = distance(motion_struct.position, player_motion.position);
					vec2 direction = normalize(player_motion.position - motion_struct.position);
					float slime_velocity = 180;

					float angle = atan2(player_motion.position.y - motion_struct.position.y, player_motion.position.x - motion_struct.position.x);
					float x_component = cos(angle) * slime_velocity;
					float y_component = sin(angle) * slime_velocity;
					

					motion_struct.velocity = { x_component, y_component };
					motion_struct.destination = motion_struct.position + (direction * 150.f);
					motion_struct.in_motion = true;
				}
				
				// if player moves out of range, return to idle animation
				if (!player_in_range(motion_struct.position, chaseRange)) {
					registry.slimeEnemies.get(slime).state = SLIME_STATE::IDLE_DOWN;
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
