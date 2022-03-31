#pragma once

// internal
#include "ai_system.hpp"
#include "combat_system.hpp"
#include "world_system.hpp"
#include "world_init.hpp"
#include "physics_system.hpp"

// check adjacent points and set a goal direction
vec2 simple_path_find(vec2 start, vec2 end, Entity enemy) {
	bool in_the_way = false;
	vec2 direction = vec2(0.0);
	float tolerance = 30;
	// check if enemy in the way
	for (Entity e : registry.enemies.entities) {
		if (e == enemy) {
			continue;
		}
		Motion& enemy_motion = registry.motions.get(e);
		vec2 enemy_pos = enemy_motion.position;
		float A = distance(start, enemy_pos);
		float B = distance(enemy_pos, end);
		float C = distance(start, end);
		if (A + B >= C - tolerance && A + B <= C + tolerance) {
			in_the_way = true;
			break;
		} else {
			in_the_way = false;
		}
	}
	if (in_the_way) {
		// perpendicular slope
		vec2 inverse_slope = normalize(vec2(end[1]-start[1], end[0]-start[0])) * vec2(100);
		vec2 pointA = end + inverse_slope;
		vec2 pointB = end - inverse_slope;
		vec2 trueDir = end-start;
		direction = (distance(start, pointA) - 30 <= distance(start, pointB)) ? pointA - start : pointB - start;
	}
	else {
		direction = end - start;
	}
	return normalize(direction);
}

void AISystem::step(Entity e)
{
	for (Entity& player : registry.players.entities) {
		if (registry.enemies.has(e)) {
			ENEMY_TYPE enemy_type = registry.enemies.get(e).type;
			switch(enemy_type) {
				case ENEMY_TYPE::SLIME:
					slime_logic(e, player);
					break;
				case ENEMY_TYPE::PLANT_SHOOTER:
					plant_shooter_logic(e, player);
					break;
				case ENEMY_TYPE::CAVELING:
					caveling_logic(e, player);
					break;
				case ENEMY_TYPE::KING_SLIME:
					king_slime_logic(e, player);
					break;
			}
		}
	}
}

void AISystem::slime_logic(Entity slime, Entity& player) {
	Motion& player_motion = registry.motions.get(player);
	Stats& stats = registry.stats.get(slime);
	float chaseRange = stats.range;
	float meleeRange = 100.f;

	Motion& motion_struct = registry.motions.get(slime);

	// Perform melee attack if close enough
	if (registry.enemies.get(slime).state == ENEMY_STATE::ATTACK) {
		if (player_in_range(motion_struct.position, meleeRange)) {
			createExplosion(world.renderer, player_motion.position);
			Mix_PlayChannel(-1, world.fire_explosion_sound, 0);
			world.logText(deal_damage(slime, player, 100));
		}
		registry.enemies.get(slime).state = ENEMY_STATE::AGGRO;
		return;
	}

	// Determine slime state
	// check if player is in range first
	if (player_in_range(motion_struct.position, chaseRange)) {
		registry.enemies.get(slime).state = ENEMY_STATE::AGGRO;
	}
	else {
		registry.enemies.get(slime).state = ENEMY_STATE::IDLE;
	}

	ENEMY_STATE state = registry.enemies.get(slime).state;
	// perform action based on state
	int dx = ichoose(irandRange(-75, -25), irandRange(25, 75));
	int dy = ichoose(irandRange(-75, -25), irandRange(25, 75));

	switch (state) {
	case ENEMY_STATE::IDLE:
		motion_struct.destination = { motion_struct.position.x + dx, motion_struct.position.y + dy };

		// Teleport if out of player sight range
		if (!player_in_range(motion_struct.position, registry.stats.get(player).range) && !player_in_range(motion_struct.destination, registry.stats.get(player).range)) {
			// temp check
			motion_struct.destination = motion_struct.position;
			motion_struct.position = { motion_struct.position.x + dx, motion_struct.position.y + dy };
			for (Entity solid : registry.solid.entities) {
				if (collides_AABB(motion_struct, registry.motions.get(solid))) {
					motion_struct.position = motion_struct.destination;
					break;
				}
			}
			motion_struct.in_motion = false;
		}
		else {
			motion_struct.velocity = 180.f * normalize(motion_struct.destination - motion_struct.position);
			motion_struct.in_motion = true;
		}
		break;
	case ENEMY_STATE::AGGRO:
		// move towards player
		for (Entity player : registry.players.entities) {
			// get player position
			Motion player_motion = registry.motions.get(player);

			// move towards player
			float slime_velocity = 180.f;
			float angle = atan2(player_motion.position.y - motion_struct.position.y, player_motion.position.x - motion_struct.position.x);

			// Special behaviour if special slime
			if (stats.range > 1000.f) {
				motion_struct.destination = { dirdist_extrapolate(motion_struct.position,
					angle + degtorad(irandRange(-10, 10)), min(400.f, dist_to(motion_struct.position,
					player_motion.position)) + irandRange(-20, -10))
				};
			}
			else {
				motion_struct.destination = { dirdist_extrapolate(motion_struct.position,
					angle + degtorad(irandRange(-10, 10)), min(140.f, dist_to(motion_struct.position,
					player_motion.position)) + irandRange(-20, -10))
				};
			}
			// Teleport if out of player sight range
			if (!player_in_range(motion_struct.position, registry.stats.get(player).range) && !player_in_range(motion_struct.destination, registry.stats.get(player).range)) {
				motion_struct.position = motion_struct.destination;
				motion_struct.in_motion = false;
			}
			else {
				vec2 direction = simple_path_find(motion_struct.position, player_motion.position, slime);
				motion_struct.velocity = slime_velocity * direction;
				motion_struct.in_motion = true;
			}
		}
		break;
	}
}

void AISystem::plant_shooter_logic(Entity plant_shooter, Entity& player) {
	Motion& player_motion = registry.motions.get(player);
	Stats& stats = registry.stats.get(plant_shooter);
	float aggroRange = stats.range;

	Motion& motion_struct = registry.motions.get(plant_shooter);

	// Resolve end-of-movement state change
	if (registry.enemies.get(plant_shooter).state == ENEMY_STATE::ATTACK) {
		registry.enemies.get(plant_shooter).state = ENEMY_STATE::AGGRO;
		return;
	}

	ENEMY_STATE& state = registry.enemies.get(plant_shooter).state;
	// Determine plant_shooter state
	// check if player is in range first
	if (player_in_range(motion_struct.position, aggroRange)) {
		state = ENEMY_STATE::AGGRO;
	}
	else {
		state = ENEMY_STATE::IDLE;
	}

	// perform action
	switch(state) {
		case ENEMY_STATE::IDLE:
			// do nothing
			break;
		case ENEMY_STATE::AGGRO:
			// Perform  attack if close enough
			if (player_in_range(motion_struct.position, aggroRange)) {
				// spawn projectile, etc
				vec2 dir = normalize(player_motion.position - motion_struct.position);
				createPlantProjectile(world.renderer, motion_struct.position + (dir*motion_struct.scale), dir, plant_shooter);
				registry.motions.get(plant_shooter).in_motion = true;
			}
			break;
		case ENEMY_STATE::DEATH:
			// death
			break;
		default:
			printf("Enemy State not supported\n");
	}
}


void AISystem::caveling_logic(Entity enemy, Entity& player) {
	Motion& player_motion = registry.motions.get(player);
	Stats& stats = registry.stats.get(enemy);
	float chaseRange = stats.range;
	float meleeRange = 100.f;

	Motion& motion_struct = registry.motions.get(enemy);

	// Perform melee attack if close enough
	if (registry.enemies.get(enemy).state == ENEMY_STATE::ATTACK) {
		if (player_in_range(motion_struct.position, meleeRange)) {
			int roll = irand(3);
			if (roll < 2) {
				createExplosion(world.renderer, player_motion.position);
				Mix_PlayChannel(-1, world.fire_explosion_sound, 0);
				world.logText(deal_damage(enemy, player, 30));
				StatusEffect poison = StatusEffect(0.3 * stats.atk, 5, StatusType::POISON, false, false);
				apply_status(player, poison);
			}
			else {
				createExplosion(world.renderer, player_motion.position);
				Mix_PlayChannel(-1, world.fire_explosion_sound, 0);
				world.logText(deal_damage(enemy, player, 100));
			}
			
		}
		registry.enemies.get(enemy).state = ENEMY_STATE::AGGRO;
		return;
	}

	// Determine enemy state
	// check if player is in range and poisoned first
	if (player_in_range(motion_struct.position, 500) && has_status(player, StatusType::POISON)) {
		registry.enemies.get(enemy).state = ENEMY_STATE::RETREAT;
	}
	else if (player_in_range(motion_struct.position, chaseRange)) {
		registry.enemies.get(enemy).state = ENEMY_STATE::AGGRO;
	}
	else {
		registry.enemies.get(enemy).state = ENEMY_STATE::IDLE;
	}

	ENEMY_STATE state = registry.enemies.get(enemy).state;
	// perform action based on state
	float angle = atan2(player_motion.position.y - motion_struct.position.y, player_motion.position.x - motion_struct.position.x);

	switch (state) {
	case ENEMY_STATE::RETREAT:

		angle += M_PI + degtorad(irandRange(-30, 30));
		motion_struct.destination = { dirdist_extrapolate(motion_struct.position, angle, min(300.f , 500.f - dist_to(motion_struct.position, player_motion.position) + irandRange(-50, 50)))};

		// Teleport if out of player sight range
		if (!player_in_range(motion_struct.position, registry.stats.get(player).range) && !player_in_range(motion_struct.destination, registry.stats.get(player).range)) {
			// temp check
			vec2 temp = motion_struct.position;
			motion_struct.position = motion_struct.destination;
			for (Entity solid : registry.solid.entities) {
				if (collides_AABB(motion_struct, registry.motions.get(solid))) {
					motion_struct.position = temp;
					break;
				}
			}
			motion_struct.in_motion = false;
		}
		else {
			motion_struct.velocity = 180.f * normalize(motion_struct.destination - motion_struct.position);
			motion_struct.in_motion = true;
		}
		break;
	case ENEMY_STATE::AGGRO:
		// move towards player
		motion_struct.destination = { dirdist_extrapolate(motion_struct.position, 
			angle + degtorad(irandRange(-10, 10)), max(20.f , dist_to(motion_struct.position, 
			player_motion.position)) + irandRange (-90, -50))};

		// Teleport if out of player sight range
		if (!player_in_range(motion_struct.position, registry.stats.get(player).range) && !player_in_range(motion_struct.destination, registry.stats.get(player).range)) {
			// temp check
			vec2 temp = motion_struct.position;
			motion_struct.position = motion_struct.destination;
			for (Entity solid : registry.solid.entities) {
				if (collides_AABB(motion_struct, registry.motions.get(solid))) {
					motion_struct.position = temp;
					break;
				}
			}
			motion_struct.in_motion = false;
		}
		else {
			motion_struct.velocity = 180.f * normalize(motion_struct.destination - motion_struct.position);
			motion_struct.in_motion = true;
		}
		break;
	}
}

void AISystem::king_slime_logic(Entity enemy, Entity& player) {
	Motion& player_motion = registry.motions.get(player);
	Boss& boss = registry.bosses.get(enemy);
	Stats& stats = registry.stats.get(enemy);
	Motion& motion_struct = registry.motions.get(enemy);
	ENEMY_STATE& state = registry.enemies.get(enemy).state;
	float aggroRange = stats.range;
	float meleeRange = 100.f;
	int num_summons = irandRange(2, 5);
	int rotation_turns = boss.num_turns % 10;

	// wake up if player is in range
	if ((player_in_range(motion_struct.position, aggroRange) && state == ENEMY_STATE::IDLE) || (stats.hp < stats.maxhp && boss.num_turns == 1)) {
		Mix_PlayMusic(world.boss0_music, -1);
		boss.counter0++;
		state = ENEMY_STATE::AGGRO;
	}
	if (rotation_turns == 3) {
		state = ENEMY_STATE::SUMMON;
	}

	// perform action (trust me, I'm not YandereDev, this is just a sequential state machine)
	switch (state) {
	case ENEMY_STATE::IDLE:
		printf("Turn Number %i: Doing Nothing!\n", boss.num_turns);
		return;
	case ENEMY_STATE::AGGRO:
		if (rotation_turns == 0 || boss.num_turns < 3) {
			printf("Turn Number %i: Doing Nothing!\n", boss.num_turns);
		}
		else if (rotation_turns == 6) {
			printf("Turn Number %i: Charging Projectile!\n", boss.num_turns);
			state = ENEMY_STATE::CHARGING_RANGED;
		}
		else if (rotation_turns == 8) {
			printf("Turn Number %i: Jumping!\n", boss.num_turns);
			state = ENEMY_STATE::LEAP;
		}
		else if (dist_to_edge(motion_struct, player_motion) <= meleeRange) {
			printf("Turn Number %i: Charging Normal Attack!\n", boss.num_turns);
			createAttackIndicator(world.renderer, motion_struct.position, motion_struct.scale.x + meleeRange * 2, motion_struct.scale.y + meleeRange * 2, true);
			state = ENEMY_STATE::CHARGING_MELEE;
		}
		else if (dist_to_edge(motion_struct, player_motion) > meleeRange) {
			printf("Turn Number %i: Charging Projectile!\n", boss.num_turns);
			state = ENEMY_STATE::CHARGING_RANGED;
		}
		break;
	case ENEMY_STATE::CHARGING_MELEE:
		printf("Turn Number %i: Doing Normal Attack!\n", boss.num_turns);
		for (Entity e : registry.attackIndicators.entities) {
			printf("Removed Attack Indicator!\n");
			registry.remove_all_components_of(e);
		}
		state = ENEMY_STATE::AGGRO;
		break;
	case ENEMY_STATE::CHARGING_RANGED:
		printf("Turn Number %i: Firing Projectile!\n", boss.num_turns);
		state = ENEMY_STATE::AGGRO;
		break;
	case ENEMY_STATE::SUMMON:
		printf("Turn Number %i: Summoning Adds!\n", boss.num_turns);
		take_damage(enemy, stats.maxhp * 0.04f * num_summons);
		while (num_summons > 0) {
			bool valid_summon = true;
			int distance = irandRange(ENEMY_BB_WIDTH * 2, ENEMY_BB_WIDTH * 3.5);
			float direction = (rand() % 360) * (M_PI / 180);
			vec2 spawnpoint = dirdist_extrapolate(motion_struct.position, direction, distance);
			Motion test = {};
			test.position = spawnpoint;
			test.scale = { ENEMY_BB_WIDTH, ENEMY_BB_HEIGHT };
			for (Entity e : registry.solid.entities) {
				if (collides_AABB(test, registry.motions.get(e))) {
					valid_summon = false;
				}
			}
			if (valid_summon) {
				boss.counter0++;
				Entity summon = createEnemy(world.renderer, spawnpoint);
				Stats& summon_stats = registry.basestats.get(summon);
				summon_stats.name = "Slime Prince " + std::to_string(boss.counter0);
				summon_stats.maxhp = 10;
				summon_stats.speed = 10;
				summon_stats.atk = 5;
				summon_stats.def = 4;
				summon_stats.range = 9999;
				registry.stats.get(summon).hp = summon_stats.maxhp;
				reset_stats(summon);
				calc_stats(summon);
				world.turnOrderSystem.turnQueue.addNewEntity(summon);
				num_summons--;
			}
		}
		registry.enemies.get(enemy).state = ENEMY_STATE::AGGRO;
		break;
	case ENEMY_STATE::LEAP:
		printf("Turn Number %i: Landing from jump!\n", boss.num_turns);
		for (Entity e : registry.attackIndicators.entities) {
			printf("Removed Attack Indicator!\n");
			registry.remove_all_components_of(e);
		}
		state = ENEMY_STATE::AGGRO;
		break;
	case ENEMY_STATE::DEATH:
		// death
		Mix_PlayMusic(world.background_music, -1);
		for (Entity e : registry.attackIndicators.entities) {
			printf("Removed Attack Indicator!\n");
			registry.remove_all_components_of(e);
		}
		break;
	default:
		printf("Enemy State not supported!\n");
	}

	boss.num_turns++;
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
