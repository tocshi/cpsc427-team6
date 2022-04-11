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
				case ENEMY_TYPE::LIVING_PEBBLE:
					living_pebble_logic(e, player);
					break;
				case ENEMY_TYPE::LIVING_ROCK:
					living_rock_logic(e, player);
					break;
				case ENEMY_TYPE::APPARITION:
					apparition_logic(e, player);
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
					angle + degtorad(irandRange(-10, 10)), min(300.f, dist_to(motion_struct.position,
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
				float dir = atan2(player_motion.position.y - motion_struct.position.y, player_motion.position.x - motion_struct.position.x);
				createProjectile(world.renderer, plant_shooter, motion_struct.position, {PLANT_PROJECTILE_BB_WIDTH, PLANT_PROJECTILE_BB_HEIGHT}, dir, 100, TEXTURE_ASSET_ID::PLANT_PROJECTILE);
				registry.solid.remove(plant_shooter);
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
	float dir = atan2(player_motion.position.y - motion_struct.position.y, player_motion.position.x - motion_struct.position.x);
	int num_summons = irandRange(2, 5);
	int rotation_turns = boss.num_turns % 10;

	// wake up if player is in range
	if ((player_in_range(motion_struct.position, aggroRange) && state == ENEMY_STATE::IDLE) || (stats.hp < stats.maxhp && boss.num_turns == 1)) {
		world.playMusic(Music::BOSS0);
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
			world.logText("The King Slime is charging up an attack!");
			Entity indicator = createAttackIndicator(world.renderer, motion_struct.position, 1000, 8, false);
			registry.renderRequests.get(indicator).used_layer = RENDER_LAYER_ID::PLAYER;
			registry.motions.get(indicator).destination = motion_struct.position;
			registry.motions.get(indicator).movement_speed = 1;
			registry.colors.insert(indicator, {1.f, 0.5f, 0.5f});
			state = ENEMY_STATE::CHARGING_RANGED;
		}
		else if (rotation_turns == 8) {
			printf("Turn Number %i: Jumping!\n", boss.num_turns);
			world.logText("The King Slime leaps into the air!");
			Entity indicator = createAttackIndicator(world.renderer, player_motion.position, motion_struct.scale.x + meleeRange * 2, motion_struct.scale.y + meleeRange * 2, true);
			Motion& indicator_motion = registry.motions.get(indicator);
			int hits = 10;
			float length = 0;
			float dir = atan2(motion_struct.position.y - player_motion.position.y, motion_struct.position.x - player_motion.position.x);
			while (hits > 4) {
				hits = 0;
				indicator_motion.position = dirdist_extrapolate(player_motion.position, dir, length);
				for (Entity i : registry.solid.entities) {
					if (registry.enemies.has(i)) { continue; }
					if (collides_AABB(indicator_motion, registry.motions.get(i))) {
						hits++;
					}
				}
				length += 10;
			}
			if (registry.wobbleTimers.has(enemy)) {
				registry.wobbleTimers.remove(enemy);
			}
			motion_struct.scale = { 0, 0 };
			motion_struct.position = { 0, 0 };
			Mix_PlayChannel(-1, world.kingslime_jump, 0);
			state = ENEMY_STATE::LEAP;
		}
		else if (dist_to_edge(motion_struct, player_motion) <= meleeRange) {
			printf("Turn Number %i: Charging Normal Attack!\n", boss.num_turns);
			world.logText("The King Slime is charging up an attack!");
			createAttackIndicator(world.renderer, motion_struct.position, motion_struct.scale.x + meleeRange * 2, motion_struct.scale.y + meleeRange * 2, true);
			state = ENEMY_STATE::CHARGING_MELEE;
		}
		else if (dist_to_edge(motion_struct, player_motion) > meleeRange) {
			printf("Turn Number %i: Charging Projectile!\n", boss.num_turns);
			world.logText("The King Slime is charging up an attack!");
			Entity indicator = createAttackIndicator(world.renderer, motion_struct.position, 1000, 8, false);
			registry.renderRequests.get(indicator).used_layer = RENDER_LAYER_ID::PLAYER;
			registry.motions.get(indicator).destination = motion_struct.position;
			registry.motions.get(indicator).movement_speed = 1;
			registry.colors.insert(indicator, { 1.f, 0.5f, 0.5f });
			state = ENEMY_STATE::CHARGING_RANGED;
		}
		break;
	case ENEMY_STATE::CHARGING_MELEE:
		printf("Turn Number %i: Doing Normal Attack!\n", boss.num_turns);
		for (int i = (int)registry.attackIndicators.components.size() - 1; i >= 0; --i) {
			if (player_in_range(motion_struct.position, registry.motions.get(registry.attackIndicators.entities[i]).scale.x / 2)) {
				world.logText(deal_damage(enemy, player, 200));
			}
			if (!registry.wobbleTimers.has(enemy)) {
				WobbleTimer& wobble = registry.wobbleTimers.emplace(enemy);
				wobble.orig_scale = motion_struct.scale;
				wobble.counter_ms = 2000;
			}
			printf("Removed Attack Indicator!\n");
			registry.remove_all_components_of(registry.attackIndicators.entities[i]);
		}
		Mix_PlayChannel(-1, world.kingslime_attack, 0);
		state = ENEMY_STATE::AGGRO;
		break;
	case ENEMY_STATE::CHARGING_RANGED:
		printf("Turn Number %i: Firing Projectile!\n", boss.num_turns);
		createProjectile(world.renderer, enemy, motion_struct.position, { 64, 34 }, dir, 120, TEXTURE_ASSET_ID::SLIMEPROJECTILE);
		registry.solid.remove(enemy);
		registry.motions.get(enemy).in_motion = true;

		// wobble for effect
		if (!registry.wobbleTimers.has(enemy)) {
			WobbleTimer& wobble = registry.wobbleTimers.emplace(enemy);
			wobble.orig_scale = motion_struct.scale;
			wobble.counter_ms = 2000;
			registry.solid.remove(enemy);
		}
		for (int i = (int)registry.attackIndicators.components.size() - 1; i >= 0; --i) {
			printf("Removed Attack Indicator!\n");
			registry.remove_all_components_of(registry.attackIndicators.entities[i]);
		}
		Mix_PlayChannel(-1, world.kingslime_attack, 0);
		state = ENEMY_STATE::AGGRO;
		break;
	case ENEMY_STATE::SUMMON:
		printf("Turn Number %i: Summoning Adds!\n", boss.num_turns);
		take_damage(enemy, min(stats.hp - 1, stats.maxhp * 0.04f * num_summons));
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
				summon_stats.range = 3000;
				registry.stats.get(summon).hp = summon_stats.maxhp;
				reset_stats(summon);
				calc_stats(summon);
				world.turnOrderSystem.turnQueue.addNewEntity(summon);
				ExpandTimer iframe = registry.iFrameTimers.emplace(summon);
				iframe.counter_ms = 50;
				num_summons--;
			}
		}
		Mix_PlayChannel(-1, world.kingslime_summon, 0);
		registry.enemies.get(enemy).state = ENEMY_STATE::AGGRO;
		break;
	case ENEMY_STATE::LEAP:
		num_summons = 0;
		printf("Turn Number %i: Landing from jump!\n", boss.num_turns);
		for (int i = (int)registry.attackIndicators.components.size() - 1; i >= 0; --i) {
			motion_struct.scale = { ENEMY_BB_WIDTH * 4, ENEMY_BB_HEIGHT * 4 };
			if (!registry.wobbleTimers.has(enemy)) {
				WobbleTimer& wobble = registry.wobbleTimers.emplace(enemy);
				wobble.orig_scale = motion_struct.scale;
				wobble.counter_ms = 2000;
				registry.solid.remove(enemy);
			}

			// absorb adds
			for (int j = (int)registry.enemies.components.size() - 1; j >= 0; --j) {
				if (registry.enemies.entities[j] != enemy
					&& collides_circle(registry.motions.get(registry.attackIndicators.entities[i]), registry.motions.get(registry.enemies.entities[j]))) {
					deal_damage(enemy, registry.enemies.entities[j], 1000);
					registry.solid.remove(registry.enemies.entities[j]);
					num_summons++;
				}
			}

			// move to destination
			motion_struct.position = registry.motions.get(registry.attackIndicators.entities[i]).position;

			// damage player
			if (player_in_range(motion_struct.position, registry.motions.get(registry.attackIndicators.entities[i]).scale.x/2)) {
				world.logText(deal_damage(enemy, player, 300));
				if (!registry.knockbacks.has(player)) {
					KnockBack& knockback = registry.knockbacks.emplace(player);
					knockback.remaining_distance = 200;
					knockback.angle = atan2(player_motion.position.y - motion_struct.position.y, player_motion.position.x - motion_struct.position.x);
				}
			}
			
			if (num_summons > 0) {
				heal(enemy, stats.maxhp * 0.03f * num_summons);
			}
			printf("Removed Attack Indicator!\n");
			registry.remove_all_components_of(registry.attackIndicators.entities[i]);
		}
		Mix_PlayChannel(-1, world.kingslime_attack, 0);
		state = ENEMY_STATE::AGGRO;
		break;
	case ENEMY_STATE::DEATH:
		// death
		world.playMusic(Music::BACKGROUND);
		for (int i = (int)registry.attackIndicators.components.size() - 1; i >= 0; --i) {
			printf("Removed Attack Indicator!\n");
			registry.remove_all_components_of(registry.attackIndicators.entities[i]);
		}
		break;
	default:
		printf("Enemy State not supported!\n");
	}

	boss.num_turns++;
}

void AISystem::living_pebble_logic(Entity enemy, Entity& player) {
	Motion& player_motion = registry.motions.get(player);
	Stats& stats = registry.stats.get(enemy);
	float chaseRange = stats.range;
	float meleeRange = 100.f;

	Motion& motion_struct = registry.motions.get(enemy);

	// Perform melee attack if close enough
	if (registry.enemies.get(enemy).state == ENEMY_STATE::ATTACK) {
		if (player_in_range(motion_struct.position, meleeRange)) {
			createExplosion(world.renderer, player_motion.position);
			Mix_PlayChannel(-1, world.fire_explosion_sound, 0);
			world.logText(deal_damage(enemy, player, 100));
		}
		registry.enemies.get(enemy).state = ENEMY_STATE::AGGRO;
		return;
	}

	// Determine enemy state
	// check if player is in range first
	if (player_in_range(motion_struct.position, chaseRange)) {
		registry.enemies.get(enemy).state = ENEMY_STATE::AGGRO;
	}
	else {
		registry.enemies.get(enemy).state = ENEMY_STATE::IDLE;
	}

	ENEMY_STATE state = registry.enemies.get(enemy).state;
	// perform action based on state

	switch (state) {
	case ENEMY_STATE::IDLE:
		motion_struct.in_motion = false;
		break;
	case ENEMY_STATE::AGGRO:
		// move towards player
		for (Entity player : registry.players.entities) {
			// get player position
			Motion player_motion = registry.motions.get(player);

			// move towards player
			float enemy_velocity = 180.f;
			float angle = atan2(player_motion.position.y - motion_struct.position.y, player_motion.position.x - motion_struct.position.x);

			// Special behaviour if special enemy
			if (stats.range > 1000.f) {
				motion_struct.destination = { dirdist_extrapolate(motion_struct.position,
					angle + degtorad(irandRange(-10, 10)), min(300.f, dist_to(motion_struct.position,
					player_motion.position)) + irandRange(-20, -10))
				};
			}
			else {
				motion_struct.destination = { dirdist_extrapolate(motion_struct.position,
					angle + degtorad(irandRange(-10, 10)), min(400.f, dist_to(motion_struct.position,
					player_motion.position)) + irandRange(-20, -10))
				};
			}
			// Teleport if out of player sight range
			if (!player_in_range(motion_struct.position, registry.stats.get(player).range) && !player_in_range(motion_struct.destination, registry.stats.get(player).range)) {
				motion_struct.position = motion_struct.destination;
				motion_struct.in_motion = false;
			}
			else {
				vec2 direction = simple_path_find(motion_struct.position, player_motion.position, enemy);
				motion_struct.velocity = enemy_velocity * direction;
				motion_struct.in_motion = true;
			}
		}
		break;
	}
}

void AISystem::living_rock_logic(Entity enemy, Entity& player) {
	Motion& player_motion = registry.motions.get(player);
	Stats& stats = registry.stats.get(enemy);
	Motion& motion_struct = registry.motions.get(enemy);
	ENEMY_STATE& state = registry.enemies.get(enemy).state;
	float aggroRange = stats.range;

	// Resolve end-of-movement state change
	if (registry.enemies.get(enemy).state == ENEMY_STATE::ATTACK) {
		registry.enemies.get(enemy).state = ENEMY_STATE::AGGRO;
		motion_struct.in_motion = false;
		return;
	}

	// Determine enemy state
	if (player_in_range(motion_struct.position, aggroRange)) {
		state = ENEMY_STATE::SUMMON;
	} else {
		state = ENEMY_STATE::IDLE;
	}

	// perform action
	switch (state) {
	case ENEMY_STATE::IDLE:
		// do nothing
		break;
	case ENEMY_STATE::SUMMON:
		bool summoned = false;
		int attempts = 20;
		while (!summoned && attempts > 0) {
			bool valid_summon = true;
			int distance = irandRange(ENEMY_BB_WIDTH * 2, ENEMY_BB_WIDTH * 2);
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
			attempts--;
			if (valid_summon) {
				Entity summon = createLivingPebble(world.renderer, spawnpoint);
				world.turnOrderSystem.turnQueue.addNewEntity(summon);
				ExpandTimer iframe = registry.iFrameTimers.emplace(summon);
				iframe.counter_ms = 50;
				summoned = true;
			}
		}
		if (summoned) {
			take_damage(enemy, 1);
		}
		break;
	}
}

void AISystem::apparition_logic(Entity enemy, Entity& player) {
	Motion& player_motion = registry.motions.get(player);
	Stats& stats = registry.stats.get(enemy);
	float chaseRange = stats.range;
	float meleeRange = 100.f;

	Motion& motion_struct = registry.motions.get(enemy);

	// Perform melee attack if close enough
	if (registry.enemies.get(enemy).state == ENEMY_STATE::ATTACK) {
		if (player_in_range(motion_struct.position, meleeRange)) {
			bool apply_blind = true;

			StatusContainer player_statuses = registry.statuses.get(player);
			for (StatusEffect s : player_statuses.statuses) {
				// check if player already has a range debuff
				if (s.effect == StatusType::RANGE_BUFF && s.value < 0) {
					apply_blind = false;
					break;
				}
			}
			if (!apply_blind) {
				createExplosion(world.renderer, player_motion.position);
				Mix_PlayChannel(-1, world.fire_explosion_sound, 0);
				world.logText(deal_damage(enemy, player, 100));
			}
			else {
				createExplosion(world.renderer, player_motion.position);
				Mix_PlayChannel(-1, world.fire_explosion_sound, 0);
				world.logText(deal_damage(enemy, player, 50));
				StatusEffect blind = StatusEffect(-0.5f, 2, StatusType::RANGE_BUFF, true, true);
				apply_status(player, blind);
			}
		}
		registry.enemies.get(enemy).state = ENEMY_STATE::AGGRO;
		return;
	}

	// Determine enemy state
	// check if player is in range first
	if (player_in_range(motion_struct.position, chaseRange)) {
		registry.enemies.get(enemy).state = ENEMY_STATE::AGGRO;
	}
	else {
		registry.enemies.get(enemy).state = ENEMY_STATE::IDLE;
	}

	ENEMY_STATE state = registry.enemies.get(enemy).state;
	// perform action based on state
	int dx = ichoose(irandRange(-75, -25), irandRange(25, 75));
	int dy = ichoose(irandRange(-75, -25), irandRange(25, 75));
	float angle = atan2(player_motion.position.y - motion_struct.position.y, player_motion.position.x - motion_struct.position.x);


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
		vec2 offset = player_motion.position - motion_struct.position;
		motion_struct.destination = { motion_struct.position + offset*(2.f/3.f) };

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
