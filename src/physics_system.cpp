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
	float dist_squared = dot(dp, dp);
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
	return motion1.position.x - bounding_box_a.x / 2 < motion2.position.x + bounding_box_b.x / 2
		&& motion1.position.x + bounding_box_a.x / 2 > motion2.position.x - bounding_box_b.x / 2
		&& motion1.position.y - bounding_box_a.y / 2 < motion2.position.y + bounding_box_b.y / 2
		&& motion1.position.y + bounding_box_a.y / 2 > motion2.position.y - bounding_box_b.y / 2;
}

bool collides_circle(const Motion& motion1, const Motion& motion2) {
	float angle = atan2(motion2.position.y - motion1.position.y, motion2.position.x - motion1.position.x);
	vec2 motion1_edge = dirdist_extrapolate(motion1.position, angle, motion1.scale.x / 2);
	vec2 motion2_edge = dirdist_extrapolate(motion2.position, angle - M_PI, motion2.scale.x / 2);
	if (dist_to(motion1.position, motion2_edge) < dist_to(motion1.position, motion1_edge)) {
		return true;
	}
	if (dist_to(motion2.position, motion1_edge) < dist_to(motion2.position, motion2_edge)) {
		return true;
	}

	return false;
}

bool collides_point_circle(vec2 position, const Motion& motion) {
	return dist_to(motion.position, position) < (motion.scale.x / 2);
}

// helper functions for collision_rotrect_circle()
bool testRectangleToPoint(float rectWidth, float rectHeight, float rectRotation, float rectCenterX, float rectCenterY, float pointX, float pointY) {
	if (rectRotation == 0)   // Higher Efficiency for Rectangles with 0 rotation.
		return abs(rectCenterX - pointX) < rectWidth / 2 && abs(rectCenterY - pointY) < rectHeight / 2;

	float tx = cos(rectRotation) * pointX - sin(rectRotation) * pointY;
	float ty = cos(rectRotation) * pointY + sin(rectRotation) * pointX;

	float cx = cos(rectRotation) * rectCenterX - sin(rectRotation) * rectCenterY;
	float cy = cos(rectRotation) * rectCenterY + sin(rectRotation) * rectCenterX;

	return abs(cx - tx) < rectWidth / 2 && abs(cy - ty) < rectHeight / 2;
}

bool testCircleToSegment(float circleCenterX, float circleCenterY, float circleRadius, float lineAX, float lineAY, float lineBX, float lineBY) {
	float lineSize = sqrt(pow(lineAX - lineBX, 2) + pow(lineAY - lineBY, 2));
	float distance;

	if (lineSize == 0) {
		distance = sqrt(pow(circleCenterX - lineAX, 2) + pow(circleCenterY - lineAY, 2));
		return distance < circleRadius;
	}

	float u = ((circleCenterX - lineAX) * (lineBX - lineAX) + (circleCenterY - lineAY) * (lineBY - lineAY)) / (lineSize * lineSize);

	if (u < 0) {
		distance = sqrt(pow(circleCenterX - lineAX, 2) + pow(circleCenterY - lineAY, 2));
	}
	else if (u > 1) {
		distance = sqrt(pow(circleCenterX - lineBX, 2) + pow(circleCenterY - lineBY, 2));
	}
	else {
		float ix = lineAX + u * (lineBX - lineAX);
		float iy = lineAY + u * (lineBY - lineAY);
		distance = sqrt(pow(circleCenterX - ix, 2) + pow(circleCenterY - iy, 2));
	}

	return distance < circleRadius;
}

// Rotated Rectangle to Circle collision check
// https://stackoverflow.com/questions/5650032/collision-detection-with-rotated-rectangles
bool collides_rotrect_circle(const Motion& rectmotion, const Motion& circlemotion) {
	float rectWidth = rectmotion.scale.x;
	float rectHeight = rectmotion.scale.y;
	float rectRotation = rectmotion.angle;
	float rectCenterX = rectmotion.position.x;
	float rectCenterY = rectmotion.position.y;
	float circleCenterX = circlemotion.position.x;
	float circleCenterY = circlemotion.position.y;
	float circleRadius = circlemotion.scale.x / 2.f;
	float tx, ty, cx, cy;

	if (rectRotation == 0) { // Higher Efficiency for Rectangles with 0 rotation.
		tx = circleCenterX;
		ty = circleCenterY;

		cx = rectCenterX;
		cy = rectCenterY;
	}
	else {
		tx = cos(rectRotation) * circleCenterX - sin(rectRotation - M_PI) * circleCenterY;
		ty = cos(rectRotation) * circleCenterY + sin(rectRotation - M_PI) * circleCenterX;

		cx = cos(rectRotation) * rectCenterX - sin(rectRotation - M_PI) * rectCenterY;
		cy = cos(rectRotation) * rectCenterY + sin(rectRotation - M_PI) * rectCenterX;
	}

	return testRectangleToPoint(rectWidth, rectHeight, rectRotation, rectCenterX, rectCenterY, circleCenterX, circleCenterY) ||
		testCircleToSegment(tx, ty, circleRadius, cx - rectWidth / 2, cy + rectHeight / 2, cx + rectWidth / 2, cy + rectHeight / 2) ||
		testCircleToSegment(tx, ty, circleRadius, cx + rectWidth / 2, cy + rectHeight / 2, cx + rectWidth / 2, cy - rectHeight / 2) ||
		testCircleToSegment(tx, ty, circleRadius, cx + rectWidth / 2, cy - rectHeight / 2, cx - rectWidth / 2, cy - rectHeight / 2) ||
		testCircleToSegment(tx, ty, circleRadius, cx - rectWidth / 2, cy - rectHeight / 2, cx - rectWidth / 2, cy + rectHeight / 2);
}

float dist_to(const vec2 position1, const vec2 position2) {
	return sqrt(pow(position2.x - position1.x, 2) + pow(position2.y - position1.y, 2));
}

float dist_to_edge(const Motion motion1, const Motion motion2) {
	return dist_to(motion1.position, motion2.position) - motion1.scale.x / 2 - motion2.scale.x / 2;
}

bool oppositeSign(vec2 v1, vec2 v2) {
	if ((v1.x < 0 && v2.x > 0) || (v1.x > 0 && v2.x < 0)) {
		return true;
	}

	if ((v1.y < 0 && v2.y > 0) || (v1.y > 0 && v2.y < 0)) {
		return true;
	}

	return false;
}

void PhysicsSystem::step(float elapsed_ms, WorldSystem* world, RenderSystem* renderer)
{
	// Resolve entity movement
	auto& motion_registry = registry.motions;
	for (uint i = 0; i < motion_registry.size(); i++)
	{
		Motion& motion = motion_registry.components[i];
		Entity entity = motion_registry.entities[i];
		float step_seconds = elapsed_ms / 1000.f;

		vec2 pos = motion.position;
		vec2 vel = motion.velocity;
		vec2 vel_for_step = vel * step_seconds;
		float vel_mag = sqrt(pow(vel.x * step_seconds, 2) + pow(vel.y * step_seconds, 2));
		vec2 dest = motion.destination;

		vec2 pos_final = { pos.x + (vel.x * step_seconds), pos.y + (vel.y * step_seconds) };

		if (registry.particles.has(entity)) {
			motion.position = pos_final;
			continue;
		}

		// trap collision/activation
		if (!registry.iFrameTimers.has(entity) && (registry.players.has(entity) || registry.enemies.has(entity))) {
			for (Entity t : registry.traps.entities) {
				// this looks stupid, but trust me, it isn't
				if (!registry.traps.has(t) || !registry.motions.has(t)) {
					continue;
				}
				Trap& trap = registry.traps.get(t);
				if (trap.triggers <= 0) {
					if (registry.motions.has(t)) {
						registry.remove_all_components_of(t);
					}
					continue;
				}
				Motion& trap_motion = registry.motions.get(t);

				if (registry.players.has(entity) && registry.enemies.has(trap.owner)) {
					if (registry.stats.get(entity).hp <= 0 || trap.triggers <= 0) { continue; }
					else if (collides_circle(trap_motion, motion)) {
						ExpandTimer iframe = registry.iFrameTimers.emplace(entity);
						iframe.counter_ms = 50;
						trigger_trap(t, entity);
						break;
					}
				}
				else if (registry.enemies.has(entity) && registry.players.has(trap.owner)) {
					if (registry.stats.get(entity).hp <= 0 || trap.triggers <= 0) { continue; }
					else if (collides_circle(trap_motion, motion)) {
						ExpandTimer iframe = registry.iFrameTimers.emplace(entity);
						iframe.counter_ms = 50;
						trigger_trap(t, entity);
						break;
					}
				}
			}
		}

		// projectile collision
		if (registry.projectileTimers.has(entity)) {
			Entity& player = registry.players.entities[0];
			Motion& player_motion = motion_registry.get(player);
			Entity& owner = registry.projectileTimers.get(entity).owner;
			ProjectileTimer& timer = registry.projectileTimers.get(entity);

			if (registry.enemies.has(owner)) {
				if (timer.counter_ms > 0 && collides_circle(motion_registry.get(entity), motion_registry.get(player))) {
					// hit player
					createExplosion(renderer, player_motion.position);
					Mix_PlayChannel(-1, world->fire_explosion_sound, 0);
					world->logText(deal_damage(owner, player, timer.multiplier));

					if (registry.enemies.get(owner).type == ENEMY_TYPE::KING_SLIME) {
						StatusEffect slimed = StatusEffect(4, 3, StatusType::SLIMED, true, true);
						if (has_status(player, StatusType::SLIMED)) { remove_status(player, StatusType::SLIMED); }
						apply_status(player, slimed);
					}
					if (registry.enemies.get(owner).type == ENEMY_TYPE::REFLEXION) {
						StatusEffect debuff = StatusEffect(-0.5, 1, StatusType::DEF_BUFF, true, true);
						apply_status(player, debuff);
					}
					timer.counter_ms = 0;
					motion_registry.get(owner).in_motion = false;
				}
			}
			else {
				for (Entity e : registry.enemies.entities) {
					Motion& enemy_motion = registry.motions.get(e);
					if (timer.counter_ms > 0 && collides_circle(motion_registry.get(entity), enemy_motion)) {
						// hit enemy
						createExplosion(renderer, enemy_motion.position);
						Mix_PlayChannel(-1, world->fire_explosion_sound, 0);
						world->logText(deal_damage(owner, e, timer.multiplier));
						timer.counter_ms = 0;
					}
				}
			}
		}

		// behaviour if currently moving
		if (vel.x * step_seconds != 0 || vel.y * step_seconds != 0) {
			bool using_astar = false;
			if (registry.aStarMotions.has(entity)) {
				using_astar = registry.aStarMotions.get(entity).using_astar;
			}
			// handle a star movement
			if (using_astar) {
				AstarMotion& aStarMotion = registry.aStarMotions.get(entity);
				vec2 deltaStart = aStarMotion.currentDest - motion.position;
				vec2 deltaEnd = aStarMotion.currentDest - pos_final;
				if (dist_to(pos_final, aStarMotion.currentDest) <= vel_mag || oppositeSign(deltaStart, deltaEnd)) {
					// if it is the final dest
					if (aStarMotion.path.size() > 0) {
						vec2 back = aStarMotion.path.back();
						motion.position = aStarMotion.currentDest;
						aStarMotion.path.pop();
						aStarMotion.currentDest = back;
						motion.destination = back;
						motion.velocity = aStarMotion.scalar_vel * normalize(back - motion.position);
					}
					else {
						motion.velocity = { 0, 0 };
						motion.destination = motion.position;
						motion.in_motion = false;
						break;
					}
				}
				//else {
				//	motion.position = pos_final;
				//	break;
				//}
			}
			else {
				if (dist_to(pos_final, dest) <= vel_mag) {
					motion.velocity = { 0, 0 };
					motion.destination = motion.position;
					motion.in_motion = false;
					break;
				}
			}
			// perform angle sweep 
			float original_angle = atan2(vel.y, vel.x) * 180 / M_PI;
			bool move_success = false;
			for (int angle = 0; angle <= 80; angle += 10) {
				for (int sign = -1; sign <= 1; sign += 2) {
					float modified_angle = original_angle + angle * sign;
					vec2 modified_velocity = { vel_mag * cos(modified_angle * M_PI / 180), vel_mag * sin(modified_angle * M_PI / 180) };
					vec2 target_position = pos + modified_velocity;
					motion.position = target_position;
					bool target_valid = true;
					for (uint j = 0; j < motion_registry.size(); j++) {

						// ignore solids if apparition
						if (registry.enemies.has(entity)) {
							if (registry.enemies.get(entity).type == ENEMY_TYPE::APPARITION) {
								break;
							}
						}

						if (j != i && registry.solid.has(motion_registry.entities[j])) {
							// differentiate between walls and non-walls
							if (registry.enemies.has(motion_registry.entities[j]) || registry.players.has(motion_registry.entities[j])) {
								Motion test = motion_registry.components[j];
								if (registry.wobbleTimers.has(motion_registry.entities[j])) {
									test.scale = registry.wobbleTimers.get(motion_registry.entities[j]).orig_scale;
								}
								if (collides_circle(motion, test)) {
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
							Entity& owner = registry.projectileTimers.get(entity).owner;
							ProjectileTimer& timer = registry.projectileTimers.get(entity);

							if (registry.enemies.has(owner)) {
								if (timer.counter_ms > 0 && collides_circle(motion_registry.get(entity), motion_registry.get(player))) {
									// hit player
									createExplosion(renderer, player_motion.position);
									Mix_PlayChannel(-1, world->fire_explosion_sound, 0);
									world->logText(deal_damage(owner, player, timer.multiplier));

									if (registry.enemies.get(owner).type == ENEMY_TYPE::KING_SLIME) {
										StatusEffect slimed = StatusEffect(4, 3, StatusType::SLIMED, true, true);
										if (has_status(player, StatusType::SLIMED)) { remove_status(player, StatusType::SLIMED); }
										apply_status(player, slimed);
									}
									if (registry.enemies.get(owner).type == ENEMY_TYPE::REFLEXION) {
										StatusEffect debuff = StatusEffect(-0.5, 1, StatusType::DEF_BUFF, true, true);
										apply_status(player, debuff);
									}
								}
								motion_registry.get(owner).in_motion = false;
							}
							else {
								for (Entity e : registry.enemies.entities) {
									Motion& enemy_motion = registry.motions.get(e);
									if (timer.counter_ms > 0 && collides_circle(motion_registry.get(entity), enemy_motion)) {
										// hit enemy
										createExplosion(renderer, enemy_motion.position);
										Mix_PlayChannel(-1, world->fire_explosion_sound, 0);
										world->logText(deal_damage(owner, e, timer.multiplier));
									}
								}
							}

							timer.counter_ms = 0;
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
					ProjectileTimer& timer = registry.projectileTimers.get(entity);
					Entity& e = registry.projectileTimers.get(entity).owner;
					motion_registry.get(e).in_motion = false;
					timer.counter_ms = 0;
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
			vel_mag = max(knock_min_velocity * step_seconds, scaled_vel * step_seconds);
		}
		else {
			vel_mag = knock_base_velocity * step_seconds;
		}
		vel_mag = min(vel_mag, knock_base_velocity * step_seconds);
		vec2 pos = motion_i.position;
		vec2 velocity = vec2(vel_mag * cos(knockback_i.angle), vel_mag * sin(knockback_i.angle));

		// perform angle sweep 
		float original_angle = knockback_i.angle * 180 / M_PI;
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
	ComponentContainer<Motion>& motion_container = registry.motions;
	for (uint i = 0; i < motion_container.components.size(); i++)
	{
		if (!registry.collidables.has(motion_container.entities[i])) {
			continue;
		}
		Motion& motion_i = motion_container.components[i];
		Entity entity_i = motion_container.entities[i];

		// note starting j at i+1 to compare all (i,j) pairs only once (and to not compare with itself)
		for (uint j = i + 1; j < motion_container.components.size(); j++)
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
			float radius = sqrt(dot(bonding_box / 2.f, bonding_box / 2.f));
			vec2 line_scale1 = { motion_i.scale.x / 10, 2 * radius };
			vec2 line_scale2 = { 2 * radius, motion_i.scale.x / 10 };
			vec2 position = motion_i.position;
			Entity line1 = createLine(motion_i.position, line_scale1);
			Entity line2 = createLine(motion_i.position, line_scale2);
		}
	}
}
