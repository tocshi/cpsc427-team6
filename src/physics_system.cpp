// internal
#include "physics_system.hpp"

// Returns the local bounding coordinates scaled by the current size of the entity
vec2 get_bounding_box(const Motion& motion)
{
	// abs is to avoid negative scale due to the facing direction.
	return { abs(motion.scale.x), abs(motion.scale.y) };
}

// This is a SUPER APPROXIMATE check that puts a circle around the bounding boxes and sees
// if the center point of either object is inside the other's bounding-box-circle. You can
// surely implement a more accurate detection
bool collides(const Motion& motion1, const Motion& motion2)
{
	vec2 dp = motion1.position - motion2.position;
	float dist_squared = dot(dp,dp);
	const vec2 other_bonding_box = get_bounding_box(motion1) / 2.f;
	const float other_r_squared = dot(other_bonding_box, other_bonding_box);
	const vec2 my_bonding_box = get_bounding_box(motion2) / 2.f;
	const float my_r_squared = dot(my_bonding_box, my_bonding_box);
	const float r_squared = max(other_r_squared, my_r_squared);
	if (dist_squared < r_squared)
		return true;
	return false;
}

bool collides_AABB(const Motion& motion1, const Motion& motion2) {
	vec2 bounding_box_a = get_bounding_box(motion1);
	vec2 bounding_box_b = get_bounding_box(motion2);
	return motion1.position.x - bounding_box_a.x/2 < motion2.position.x + bounding_box_b.x/2
		&& motion1.position.x + bounding_box_a.x/2 > motion2.position.x - bounding_box_b.x/2
		&& motion1.position.y - bounding_box_a.y/2 < motion2.position.y + bounding_box_b.y/2
		&& motion1.position.y + bounding_box_a.y/2 > motion2.position.y - bounding_box_b.y/2;
}

bool collides_circle(const Motion& motion1, const Motion& motion2) {
	float angle = atan2(motion2.position.y - motion1.position.y, motion2.position.x - motion1.position.x);
	vec2 motion1_edge = dirdist_extrapolate(motion1.position, angle, motion1.scale.x / 2);
	vec2 motion2_edge = dirdist_extrapolate(motion2.position, angle - M_PI, motion2.scale.x / 2);
	if (dist_to(motion1.position, motion2_edge) < dist_to(motion1.position, motion1_edge)) {
		return true;
	}
	else {
		return false;
	}
}

float dist_to(const vec2 position1, const vec2 position2) {
	return sqrt(pow(position2.x - position1.x, 2) + pow(position2.y - position1.y, 2));
}

void PhysicsSystem::step(float elapsed_ms, WorldSystem* world, RenderSystem* renderer)
{
	// Resolve entity movement
	auto& motion_registry = registry.motions;
	for(uint i = 0; i< motion_registry.size(); i++)
	{
		Motion& motion = motion_registry.components[i];
		Entity entity = motion_registry.entities[i];
		float step_seconds = elapsed_ms / 1000.f;

		vec2 pos = motion.position;
		vec2 vel = motion.velocity;
		vec2 vel_for_step = vel * step_seconds;
		float vel_mag = sqrt(pow(vel.x * step_seconds, 2) + pow(vel.y * step_seconds, 2));
		vec2 dest = motion.destination;

		vec2 pos_final = {pos.x + (vel.x * step_seconds), pos.y + (vel.y * step_seconds)};

		// projectile collision
		if (registry.projectileTimers.has(entity)) {
			Entity player = registry.players.entities[0];
			Motion& player_motion = motion_registry.get(player);
			if (collides_circle(motion_registry.get(entity), motion_registry.get(player))) {
				// hit player
				Entity enemy = registry.projectileTimers.get(entity).owner;
				createExplosion(renderer, player_motion.position);
				Mix_PlayChannel(-1, world->fire_explosion_sound, 0);
				world->logText(deal_damage(enemy, player, 100));
				Entity& e = registry.projectileTimers.get(entity).owner;
				motion_registry.get(e).in_motion = false;
				registry.remove_all_components_of(entity);
			}
		}

		// behaviour if currently moving
		if (vel.x * step_seconds != 0 || vel.y * step_seconds != 0) {
			if (dist_to(pos_final, dest) <= vel_mag) {
				motion.velocity = { 0, 0 };
				motion.destination = motion.position;
				motion.in_motion = false;
				break;
			}
			// perform angle sweep 
			float original_angle = atan2(vel.y, vel.x) * 180 / M_PI;
			bool move_success = false;
			for (int angle = 0; angle <= 80; angle += 10) {
				for (int sign = -1; sign <= 1; sign += 2) {
					float modified_angle = original_angle + angle * sign;
					vec2 modified_velocity = { vel_mag * cos(modified_angle * M_PI/180), vel_mag * sin(modified_angle * M_PI / 180) };
					vec2 target_position = pos + modified_velocity;
					motion.position = target_position;
					bool target_valid = true;
					for (uint j = 0; j < motion_registry.size(); j++) {
						if (j != i && registry.solid.has(motion_registry.entities[j])) {
							// differentiate between walls and non-walls
							if (registry.enemies.has(motion_registry.entities[j]) || registry.players.has(motion_registry.entities[j])) {
								if (collides_circle(motion, motion_registry.components[j])) {
									target_valid = false;
									break;
								}
							}
							else if (collides_AABB(motion, motion_registry.components[j])) {
								target_valid = false;
								break;
							}
						}
					}
					if (target_valid) {
						move_success = true;

						float speed = motion.movement_speed;
						float angle_to_dest = atan2(dest.y - motion.position.y, dest.x - motion.position.x) * 180 / M_PI;
						float angle_diff = abs(modified_angle - angle_to_dest);
						if (angle_diff > 80.f && angle_diff < 100.f) {
							motion.position = pos;
							motion.destination = motion.position;
							motion.velocity = { 0,0 };
							motion.in_motion = false;
						}
						break;
					}
					// projectile hit wall
					if (!target_valid) {
						if (registry.projectileTimers.has(entity)) {
							Entity& player = registry.players.entities[0];
							Motion& player_motion = motion_registry.get(player);
							Entity& enemy = registry.projectileTimers.get(entity).owner;

							// did it hit player?
							if (collides_circle(motion_registry.get(entity), motion_registry.get(player))) {
								createExplosion(renderer, player_motion.position);
								Mix_PlayChannel(-1, world->fire_explosion_sound, 0);
								world->logText(deal_damage(enemy, player, 100));
							}

							motion_registry.get(enemy).in_motion = false;
							registry.remove_all_components_of(entity);
							break;
						}
					}
				}
				if (move_success) {
					float speed = motion.movement_speed;
					float angle = atan2(dest.y - pos.y, dest.x - pos.x);
					float x_component = cos(angle) * speed;
					float y_component = sin(angle) * speed;
					motion.velocity = { x_component, y_component };
					break;
				}
			}
			if (!move_success) {
				motion.position = pos;
				motion.destination = motion.position;
				motion.velocity = { 0,0 };
				motion.in_motion = false;
				if (registry.projectileTimers.has(entity)) {
					Entity& e = registry.projectileTimers.get(entity).owner;
					motion_registry.get(e).in_motion = false;
					registry.remove_all_components_of(entity);
				}
			}
		}
	}

	// Resolve knockback effects
	for (int i = registry.knockbacks.size() - 1; i >= 0; i--) {
		float step_seconds = elapsed_ms / 1000.f;
		Entity entity_i = registry.knockbacks.entities[i];
		if (!registry.motions.has(entity_i))
			continue;
		Motion& motion_i = registry.motions.get(entity_i);
		KnockBack& knockback_i = registry.knockbacks.components[i];
		if (knockback_i.remaining_distance < 0.01f) {
			registry.knockbacks.remove(entity_i);
			continue;
		}
		float vel_mag;
		if (knockback_i.remaining_distance < knock_decel_threshold) {
			float scaled_vel = knock_min_velocity + (knock_base_velocity - knock_min_velocity) * (1 - (knock_decel_threshold - knockback_i.remaining_distance) / knock_decel_threshold);
			vel_mag = max(knock_min_velocity*step_seconds, scaled_vel * step_seconds);
		}
		else {
			vel_mag = knock_base_velocity * step_seconds;
		}
		vel_mag = min(vel_mag, knock_base_velocity * step_seconds);
		vec2 pos = motion_i.position;
		vec2 velocity = vec2(vel_mag * cos(knockback_i.angle), vel_mag * sin(knockback_i.angle));

		// perform angle sweep 
		float original_angle = knockback_i.angle * 180/M_PI;
		bool move_success = false;
		for (int angle = 0; angle <= 60; angle += 10) {
			for (int sign = -1; sign <= 1; sign += 2) {
				float modified_angle = original_angle + angle * sign;
				vec2 modified_velocity = { vel_mag * cos(modified_angle * M_PI / 180), vel_mag * sin(modified_angle * M_PI / 180) };
				vec2 target_position = pos + modified_velocity;
				motion_i.position = target_position;
				bool target_valid = true;
				for (uint j = 0; j < motion_registry.size(); j++) {
					if (motion_registry.entities[j] != entity_i && registry.solid.has(motion_registry.entities[j])) {
						// differentiate between walls and non-walls
						if (registry.enemies.has(motion_registry.entities[j]) || registry.players.has(motion_registry.entities[j])) {
							if (collides_circle(motion_i, motion_registry.components[j])) {
								target_valid = false;
								break;
							}
						}
						else if (collides_AABB(motion_i, motion_registry.components[j])) {
							target_valid = false;
							break;
						}
					}
				}
				if (target_valid) {
					move_success = true;
					break;
				}
			}
			if (move_success) {
				knockback_i.remaining_distance -= vel_mag;
				break;
			}
		}
		if (!move_success) {
			motion_i.position = pos;
			registry.knockbacks.remove(entity_i);
		}
	}

	// Check for collisions between all moving entities
    ComponentContainer<Motion> &motion_container = registry.motions;
	for(uint i = 0; i<motion_container.components.size(); i++)
	{
		if (!registry.collidables.has(motion_container.entities[i])) {
			continue;
		}
		Motion& motion_i = motion_container.components[i];
		Entity entity_i = motion_container.entities[i];
		
		// note starting j at i+1 to compare all (i,j) pairs only once (and to not compare with itself)
		for(uint j = i+1; j<motion_container.components.size(); j++)
		{
			if (!registry.collidables.has(motion_container.entities[j])) {
				continue;
			}
			Motion& motion_j = motion_container.components[j];
			if (collides(motion_i, motion_j))
			{
				Entity entity_j = motion_container.entities[j];
				// Create a collisions event
				// We are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity
				registry.collisions.emplace_with_duplicates(entity_i, entity_j);
				registry.collisions.emplace_with_duplicates(entity_j, entity_i);
			}
		}
	}

	// you may need the following quantities to compute wall positions
	(float)window_width_px; (float)window_height_px;

	// debugging of bounding boxes
	if (debugging.in_debug_mode)
	{
		uint size_before_adding_new = (uint)motion_container.components.size();
		for (uint i = 0; i < size_before_adding_new; i++)
		{
			Motion& motion_i = motion_container.components[i];
			Entity entity_i = motion_container.entities[i];

			// don't draw debugging visuals around debug lines
			if (registry.debugComponents.has(entity_i))
				continue;

			// visualize the radius with two axis-aligned lines
			const vec2 bonding_box = get_bounding_box(motion_i);
			float radius = sqrt(dot(bonding_box/2.f, bonding_box/2.f));
			vec2 line_scale1 = { motion_i.scale.x / 10, 2*radius };
			vec2 line_scale2 = { 2*radius, motion_i.scale.x / 10};
			vec2 position = motion_i.position;
			Entity line1 = createLine(motion_i.position, line_scale1);
			Entity line2 = createLine(motion_i.position, line_scale2);
		}
	}
}