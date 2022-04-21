#pragma once

// internal
#include "ai_system.hpp"
#include "combat_system.hpp"
#include "world_system.hpp"
#include "world_init.hpp"
#include "physics_system.hpp"

// returns true if there is a (non-player) entity at the current location
bool entityAtLocation(Entity& main, Motion& enemyMotion) {
	// walls
	for (Entity w : registry.collidables.entities) {
		Motion& wallMotion = registry.motions.get(w);

		// if colliding with wall, return true;
		if (collides_AABB(enemyMotion, wallMotion)) {
			return true;
		}
	}

	// other enemies (slimes)
	for (Entity e : registry.enemies.entities) {
		// only check for collisions with other entities
		if (main != e) {
			Motion& enemy2Motion = registry.motions.get(e);

			// if colliding with enemy, return true;
			if (collides_circle(enemyMotion, enemy2Motion)) {
				return true;
			}
		}
	}

	// bosses
	for (Entity b : registry.bosses.entities) {
		// only check for collisions with other entities
		if (main != b) {
			Motion& bossMotion = registry.motions.get(b);

			// if colliding with enemy, return true;
			if (collides_circle(enemyMotion, bossMotion)) {
				return true;
			}
		}
	}

	return false;
}

// returns true if the given node is in the list based on the position of the nodes
AstarNode* nodeInList(AstarNode* node, std::vector<AstarNode*> list) {
	for (int i = 0; i < list.size(); i++) {
		if (list[i]->position == node->position) {
			return list[i];
		}
	}
	return nullptr;
}

void removeFromList(AstarNode* node, std::vector<AstarNode*>* list) {
	std::vector<AstarNode*>& listRef = *list;
	for (int i = 0; i < listRef.size(); i++) {
		if (listRef[i] == node) {
			listRef.erase(listRef.begin() + i);
			return;
		}
	}
}

// returns the node with the lowest f_cost in the given list
AstarNode* getLowestCostNodeInList(std::vector<AstarNode*> list) {
	AstarNode* lowest = list[0];
	for (int i = 1; i < list.size(); i++) {
		if (list[i]->f_cost < lowest->f_cost) {
			lowest = list[i];
		}
	}
	return lowest;
}

// returns the node with the lowest h_cost in the given list
AstarNode* getLowestHCostNodeInList(std::vector<AstarNode*> list) {
	AstarNode* lowest = list[0];
	for (int i = 1; i < list.size(); i++) {
		if (list[i]->h_cost < lowest->h_cost) {
			lowest = list[i];
		}
	}
	return lowest;
}

// returns true if the distance to the node is greater than the range
bool nodeOutRange(vec2 enemyPos, vec2 nodePos, float range) {
	return sqrt(pow(enemyPos.x - nodePos.x, 2) + pow(enemyPos.y - nodePos.y, 2)) > range;
	//return  false;
}

// Astar returns a root AstarNode, then the ai step exectutes a move to each of those steps in sequence
std::vector<AstarNode*> AstarPathfinding(Entity enemy, float range) {
	// initialize the open and closed lists
	std::vector<AstarNode*> openSet;
	std::vector<AstarNode*> closedSet;

	Motion enemyMotion = registry.motions.get(enemy);
	vec2 enemyPos = enemyMotion.position;

	// get player position
	vec2 playerPos = vec2(0.f, 0.f);
	for (Entity p : registry.players.entities) {
		playerPos = registry.motions.get(p).position;
	}

	AstarNode* startNode = new AstarNode;
	startNode->position = enemyPos;

	openSet.push_back(startNode);

	AstarNode* endNode = 0;
	AstarNode* currNode = 0;

	float step_range = 64.f;



	while (openSet.size() > 0) {
		// get the next node to look at on the list
		currNode = getLowestCostNodeInList(openSet);

		removeFromList(currNode, &openSet);

		closedSet.push_back(currNode);

		// need to check each of the squares + 32 pixels away

		// 10 g_cost for the 4 sides
		
		// if the node is at the player break
		if (currNode->h_cost <= step_range) {
			printf("here top level");
			endNode = currNode;
			break;
		}

		Motion nodeMotion = Motion();
		nodeMotion.position = currNode->position + vec2(step_range, 0.f);
		nodeMotion.position = enemyMotion.scale;
		
		// right side
		if (!nodeOutRange(enemyPos, currNode->position + vec2(step_range, 0.f), range) && !entityAtLocation(enemy, nodeMotion)) {
			AstarNode* node = new AstarNode;
			node->position = currNode->position + vec2(step_range, 0.f);
			node->parent = currNode;
			node->g_cost = currNode->g_cost + 10.f;
			// h_cost is the distance to the player
			node->h_cost = sqrt(pow(playerPos.x - node->position.x, 2) + pow(playerPos.y - node->position.y, 2));
			node->f_cost = node->g_cost + node->h_cost;

			// if the node is at the player break
			if (node->h_cost <= step_range) {
				printf("here right");
				endNode = new AstarNode;
				endNode->position = node->position;
				endNode->parent = node->parent;
				endNode->g_cost = node->g_cost;
				endNode->f_cost = node->f_cost;
				delete node;
				break;
			}

			// don't look at the node if it's in the closedSet
			if (!nodeInList(node, closedSet)) {
				AstarNode* list_node = nodeInList(node, openSet);
				if (list_node) {
					if (node->g_cost < list_node->g_cost) {
						// remove the old position from the openSet
						removeFromList(list_node, &openSet);
						// add the new node to the list instead
						delete list_node;
						openSet.push_back(node);
					}
					else {
						delete node;
					}
				}
				else {
					openSet.push_back(node);
				}
			}			
		}

		// left side
		nodeMotion.position = currNode->position - vec2(step_range, 0.f);
		if (!nodeOutRange(enemyPos, currNode->position - vec2(step_range, 0.f), range) && !entityAtLocation(enemy, nodeMotion)) {
			AstarNode* node = new AstarNode;
			node->position = currNode->position - vec2(step_range, 0.f);
			node->parent = currNode;
			node->g_cost = currNode->g_cost + 10.f;
			// h_cost is the distance to the player
			node->h_cost = sqrt(pow(playerPos.x - node->position.x, 2) + pow(playerPos.y - node->position.y, 2));
			node->f_cost = node->g_cost + node->h_cost;

			// if the node is at the player break
			if (node->h_cost <= step_range) {
				printf("here left");
				endNode = new AstarNode;
				endNode->position = node->position;
				endNode->parent = node->parent;
				endNode->g_cost = node->g_cost;
				endNode->f_cost = node->f_cost;
				delete node;
				break;
			}

			// don't look at the node if it's in the closedSet
			if (!nodeInList(node, closedSet)) {
				AstarNode* list_node = nodeInList(node, openSet);
				if (list_node) {
					if (node->g_cost < list_node->g_cost) {
						// remove the old position from the openSet
						removeFromList(list_node, &openSet);
						// add the new node to the list instead
						delete list_node;
						openSet.push_back(node);
					}
					else {
						delete node;
					}
				}
				else {
					openSet.push_back(node);
				}
			}
		}

		// top
		nodeMotion.position = currNode->position - vec2(0.f, step_range);
		if (!nodeOutRange(enemyPos, currNode->position - vec2(0.f, step_range), range) && !entityAtLocation(enemy, nodeMotion)) {
			AstarNode* node = new AstarNode;
			node->position = currNode->position - vec2(0.f, step_range);
			node->parent = currNode;
			node->g_cost = currNode->g_cost + 10.f;
			// h_cost is the distance to the player
			node->h_cost = sqrt(pow(playerPos.x - node->position.x, 2) + pow(playerPos.y - node->position.y, 2));
			node->f_cost = node->g_cost + node->h_cost;

			// if the node is at the player break
			if (node->h_cost <= step_range) {
				printf("here top");
				endNode = new AstarNode;
				endNode->position = node->position;
				endNode->parent = node->parent;
				endNode->g_cost = node->g_cost;
				endNode->f_cost = node->f_cost;
				delete node;
				break;
			}

			// don't look at the node if it's in the closedSet
			if (!nodeInList(node, closedSet)) {
				AstarNode* list_node = nodeInList(node, openSet);
				if (list_node) {
					if (node->g_cost < list_node->g_cost) {
						// remove the old position from the openSet
						removeFromList(list_node, &openSet);
						// add the new node to the list instead
						delete list_node;
						openSet.push_back(node);
					}
					else {
						delete node;
					}
				}
				else {
					openSet.push_back(node);
				}
			}
		}

		// bottom
		nodeMotion.position = currNode->position + vec2(0.f, step_range);
		if (!nodeOutRange(enemyPos, currNode->position + vec2(0.f, step_range), range) && !entityAtLocation(enemy, nodeMotion)) {
			AstarNode* node = new AstarNode;
			node->position = currNode->position + vec2(0.f, step_range);
			node->parent = currNode;
			node->g_cost = currNode->g_cost + 10.f;
			// h_cost is the distance to the player
			node->h_cost = sqrt(pow(playerPos.x - node->position.x, 2) + pow(playerPos.y - node->position.y, 2));
			node->f_cost = node->g_cost + node->h_cost;

			// if the node is at the player break
			if (node->h_cost <= step_range) {
				printf("here bottom");
				endNode = new AstarNode;
				endNode->position = node->position;
				endNode->parent = node->parent;
				endNode->g_cost = node->g_cost;
				endNode->f_cost = node->f_cost;
				delete node;
				break;
			}

			// don't look at the node if it's in the closedSet
			if (!nodeInList(node, closedSet)) {
				AstarNode* list_node = nodeInList(node, openSet);
				if (list_node) {
					if (node->g_cost < list_node->g_cost) {
						// remove the old position from the openSet
						removeFromList(list_node, &openSet);
						// add the new node to the list instead
						delete list_node;
						openSet.push_back(node);
					}
					else {
						delete node;
					}
				}
				else {
					openSet.push_back(node);
				}
			}
		}

		// 14 g_cost for the diagonals

		// top right
		nodeMotion.position = currNode->position + vec2(step_range, -step_range);
		if (!nodeOutRange(enemyPos, currNode->position + vec2(step_range, -step_range), range) && !entityAtLocation(enemy, nodeMotion)) {
			AstarNode* node = new AstarNode;
			node->position = currNode->position + vec2(step_range, -step_range);
			node->parent = currNode;
			node->g_cost = currNode->g_cost + 14.f;
			// h_cost is the distance to the player
			node->h_cost = sqrt(pow(playerPos.x - node->position.x, 2) + pow(playerPos.y - node->position.y, 2));
			node->f_cost = node->g_cost + node->h_cost;

			// if the node is at the player break
			if (node->h_cost <= step_range) {
				printf("here top right");
				endNode = new AstarNode;
				endNode->position = node->position;
				endNode->parent = node->parent;
				endNode->g_cost = node->g_cost;
				endNode->f_cost = node->f_cost;
				delete node;
				break;
			}

			// don't look at the node if it's in the closedSet
			if (!nodeInList(node, closedSet)) {
				AstarNode* list_node = nodeInList(node, openSet);
				if (list_node) {
					if (node->g_cost < list_node->g_cost) {
						// remove the old position from the openSet
						removeFromList(list_node, &openSet);
						// add the new node to the list instead
						openSet.push_back(node);
						delete list_node;
					}
					else {
						delete node;
					}
				}
				else {
					openSet.push_back(node);
				}
			}
		}

		// top left
		nodeMotion.position = currNode->position - vec2(step_range, step_range);
		if (!nodeOutRange(enemyPos, currNode->position - vec2(step_range, step_range), range) && !entityAtLocation(enemy, nodeMotion)) {
			AstarNode* node = new AstarNode;
			node->position = currNode->position - vec2(step_range, step_range);
			node->parent = currNode;
			node->g_cost = currNode->g_cost + 14.f;
			// h_cost is the distance to the player
			node->h_cost = sqrt(pow(playerPos.x - node->position.x, 2) + pow(playerPos.y - node->position.y, 2));
			node->f_cost = node->g_cost + node->h_cost;

			// if the node is at the player break
			if (node->h_cost <= step_range) {
				printf("here top left");
				endNode = new AstarNode;
				endNode->position = node->position;
				endNode->parent = node->parent;
				endNode->g_cost = node->g_cost;
				endNode->f_cost = node->f_cost;
				delete node;
				break;
			}

			// don't look at the node if it's in the closedSet
			if (!nodeInList(node, closedSet)) {
				AstarNode* list_node = nodeInList(node, openSet);
				if (list_node) {
					if (node->g_cost < list_node->g_cost) {
						// remove the old position from the openSet
						removeFromList(list_node, &openSet);
						delete list_node;
						// add the new node to the list instead
						openSet.push_back(node);
					}
					else {
						delete node;
					}
				}
				else {
					openSet.push_back(node);
				}
			}
		}

		// bottom right
		nodeMotion.position = currNode->position + vec2(step_range, step_range);
		if (!nodeOutRange(enemyPos, currNode->position + vec2(step_range, step_range), range) && !entityAtLocation(enemy, nodeMotion)) {
			AstarNode* node = new AstarNode;
			node->position = currNode->position + vec2(step_range, step_range);
			node->parent = currNode;
			node->g_cost = currNode->g_cost + 14.f;
			// h_cost is the distance to the player
			node->h_cost = sqrt(pow(playerPos.x - node->position.x, 2) + pow(playerPos.y - node->position.y, 2));
			node->f_cost = node->g_cost + node->h_cost;

			// if the node is at the player break
			if (node->h_cost <= step_range) {
				printf("here bottom right");
				endNode = new AstarNode;
				endNode->position = node->position;
				endNode->parent = node->parent;
				endNode->g_cost = node->g_cost;
				endNode->f_cost = node->f_cost;
				delete node;
				break;
			}

			// don't look at the node if it's in the closedSet
			if (!nodeInList(node, closedSet)) {
				AstarNode* list_node = nodeInList(node, openSet);
				if (list_node) {
					if (node->g_cost < list_node->g_cost) {
						// remove the old position from the openSet
						removeFromList(list_node, &openSet);
						// add the new node to the list instead
						delete list_node;
						openSet.push_back(node);
					}
					else {
						delete node;
					}
				}
				else {
					openSet.push_back(node);
				}
			}
		}

		// bottom left
		nodeMotion.position = currNode->position + vec2(-step_range, step_range);
		if (!nodeOutRange(enemyPos, currNode->position + vec2(-step_range, step_range), range) && !entityAtLocation(enemy, nodeMotion)) {
			AstarNode* node = new AstarNode;
			node->position = currNode->position + vec2(-step_range, step_range);
			node->parent = currNode;
			node->g_cost = currNode->g_cost + 14.f;
			// h_cost is the distance to the player
			node->h_cost = sqrt(pow(playerPos.x - node->position.x, 2) + pow(playerPos.y - node->position.y, 2));
			node->f_cost = node->g_cost + node->h_cost;

			// if the node is at the player break
			if (node->h_cost <= step_range) {
				printf("here bottom left");
				endNode = new AstarNode;
				endNode->position = node->position;
				endNode->parent = node->parent;
				endNode->g_cost = node->g_cost;
				endNode->f_cost = node->f_cost;
				delete node;
				break;
			}

			// don't look at the node if it's in the closedSet
			if (!nodeInList(node, closedSet)) {
				AstarNode* list_node = nodeInList(node, openSet);
				if (list_node) {
					if (node->g_cost < list_node->g_cost) {
						// remove the old position from the openSet
						removeFromList(list_node, &openSet);
						// add the new node to the list instead
						delete list_node;
						openSet.push_back(node);
					}
					else {
						delete node;
					}
				}
				else {
					openSet.push_back(node);
				}
			}
		}
	}

	std::vector<AstarNode*> pathVector;
	if (endNode == 0 || endNode->parent == 0) {
		// if player hasn't been reached, pick the closest node visited
		endNode = getLowestHCostNodeInList(closedSet);
		if (endNode->position == enemyPos) {
			return pathVector;
		}
	}

	endNode = endNode->parent;
	AstarNode* childNode = endNode;
	// need to reverse the endNode
	AstarNode* reverseNode = endNode;
	if (endNode->parent != 0) {
		reverseNode = endNode->parent;
		while (reverseNode != 0 || childNode != 0) {
			reverseNode->children.push_back(childNode);
			if (reverseNode->parent == 0) {
				break;
			}
			else {
				reverseNode = reverseNode->parent;
				childNode = childNode->parent;
			}
		}
	}


	if (endNode->position == enemyPos) {
		return pathVector;
	}
	pathVector.push_back(reverseNode);
	while (reverseNode->children.size() > 0) {
		pathVector.push_back(reverseNode->children[0]);
		reverseNode = reverseNode->children[0];
	}

	// return the path vector
	return pathVector;
}

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
				case ENEMY_TYPE::REFLEXION:
					reflexion_logic(e, player);
					break;
			}
		}
	}
}

void AISystem::slime_logic(Entity slime, Entity& player) {
	Motion& player_motion = registry.motions.get(player);
	Stats& stats = registry.stats.get(slime);
	float chaseRange = stats.range;
	// float chaseRange = 1000.f;
	float meleeRange = 100.f;
	float dir = irand(360) * M_PI / 180;
	float dist = irandRange(70, 140);

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

	// set initial a star stat
	AstarMotion& aStarMotion = registry.aStarMotions.get(slime);
	aStarMotion.using_astar = false;

	switch (state) {
	case ENEMY_STATE::IDLE:
		motion_struct.destination = dirdist_extrapolate(motion_struct.position, dir, dist);
		// Teleport if out of player sight range
		if (!player_in_range(motion_struct.position, registry.stats.get(player).range) && !player_in_range(motion_struct.destination, registry.stats.get(player).range)) {
			// temp check
			motion_struct.destination = motion_struct.position;
			motion_struct.position = dirdist_extrapolate(motion_struct.position, dir, dist);
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
			if (stats.range > 2999.f) {
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
				std::vector<AstarNode*> starVector = AstarPathfinding(slime, chaseRange);
				
				if (starVector.size() == 0 || !useSlimeAStar) {
					vec2 direction = simple_path_find(motion_struct.position, player_motion.position, slime);
					motion_struct.velocity = slime_velocity * direction;
					motion_struct.in_motion = true;
				}
				else {

					aStarMotion.scalar_vel = slime_velocity;
					aStarMotion.using_astar = true;

					for (int i = 0; i < starVector.size(); i++) {
						aStarMotion.path.push(starVector[i]->position);
						// Uncomment the below to see the generated path
						//Entity test = createBigSlash(world.renderer, starVector[i]->position, 0, 0);
						//registry.renderRequests.get(test).used_texture = TEXTURE_ASSET_ID::ARTIFACT_PLACEHOLDER;
						//registry.motions.get(test).scale = vec2(50.f, 50.f);
						//registry.expandTimers.get(test).counter_ms = 20000;
						if (i == 1) {
							motion_struct.destination = starVector[i]->position;
							aStarMotion.currentDest = starVector[i]->position;
							motion_struct.velocity = slime_velocity * normalize(starVector[i]->position - motion_struct.position);
						}
						delete starVector[i];
					}
					motion_struct.in_motion = true;
				}
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

				// play shoot sound
				world.playPlantShootSound();
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

	// set initial a star stat
	AstarMotion& aStarMotion = registry.aStarMotions.get(enemy);
	aStarMotion.using_astar = false;

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
			// move towards player
			std::vector<AstarNode*> starVector = AstarPathfinding(enemy, chaseRange);
			if (starVector.size() == 0) {
				motion_struct.destination = { dirdist_extrapolate(motion_struct.position,
				angle + degtorad(irandRange(-10, 10)), max(20.f , dist_to(motion_struct.position,
				player_motion.position)) + irandRange(-90, -50)) 
				};
				motion_struct.velocity = 180.f * normalize(motion_struct.destination - motion_struct.position);
				motion_struct.in_motion = true;
			}
			else {

				aStarMotion.scalar_vel = 180.f;
				aStarMotion.using_astar = true;

				for (int i = 0; i < starVector.size(); i++) {
					aStarMotion.path.push(starVector[i]->position);
					// Uncomment the below to see the generated path
					// Entity test = createBigSlash(world.renderer, starVector[i]->position, 0, 0);
					// registry.renderRequests.get(test).used_texture = TEXTURE_ASSET_ID::ARTIFACT_PLACEHOLDER;
					// registry.motions.get(test).scale = vec2(50.f, 50.f);
					// registry.expandTimers.get(test).counter_ms = 20000;
					if (i == 1) {
						motion_struct.destination = starVector[i]->position;
						aStarMotion.currentDest = starVector[i]->position;
						motion_struct.velocity = 180.f * normalize(starVector[i]->position - motion_struct.position);
					}
					delete starVector[i];
				}
				motion_struct.in_motion = true;
			}
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
			registry.colors.insert(indicator, {1.f, 0.5f, 0.5f, 1.f});
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
			registry.colors.insert(indicator, { 1.f, 0.5f, 0.5f, 1.f });
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
					take_damage(registry.enemies.entities[j], 999);
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
			if (stats.range > 2999.f) {
				Entity explosion = createExplosion(world.renderer, motion_struct.position);
				registry.motions.get(explosion).scale = { 300, 300 };
				registry.colors.insert(explosion, { 1.f, 0.f, 0.f, 1.f });
				take_damage(enemy, 9999);
			}
			else {
				createExplosion(world.renderer, player_motion.position);
			}
			Mix_PlayChannel(-1, world.fire_explosion_sound, 0);
			world.logText(deal_damage(enemy, player, 100));
		}

		// Special enemy
		if (stats.range > 2999.f && stats.hp > 0) {
			take_damage(enemy, 999);
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
			if (stats.range > 2999.f) {
				motion_struct.destination = { dirdist_extrapolate(motion_struct.position,
					angle + degtorad(irandRange(-10, 10)), min(140.f, dist_to(motion_struct.position,
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
		int attempts = 50;
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

				// play rock summon sound
				world.playRockSummonSound();
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
				StatusEffect blind = StatusEffect(-0.5f, 1, StatusType::RANGE_BUFF, true, true);
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

void AISystem::reflexion_logic(Entity enemy, Entity& player) {
	Motion& player_motion = registry.motions.get(player);
	Boss& boss = registry.bosses.get(enemy);
	Stats& stats = registry.stats.get(enemy);
	Motion& motion_struct = registry.motions.get(enemy);
	ENEMY_STATE& state = registry.enemies.get(enemy).state;
	int roll = irand(3);
	float aggroRange = stats.range;
	float meleeRange = 100.f;
	float dir = atan2(player_motion.position.y - motion_struct.position.y, player_motion.position.x - motion_struct.position.x);

	// Stand Your Ground
	if (registry.hidden.has(enemy) && (state == ENEMY_STATE::AGGRO || state == ENEMY_STATE::ATTACK)) {
		printf("Turn Number %i: Stand Your Ground!\n", boss.num_turns);
		// Random Dialogue
		roll = irand(3);
		if (roll < 1) { world.logText("???: Running won't do you any good!", { 1.0, 0.2, 0.2 }); }
		else if (roll < 2) { world.logText("???: Don't let me out of your sight!", { 1.0, 0.2, 0.2 }); }
		else { world.logText("???: You can't escape your fate!", { 1.0, 0.2, 0.2 }); }

		// teleporto
		bool valid = false;
		int attempts = 50;
		while (!valid && attempts > 0) {
			bool valid_tp = true;
			int distance = irandRange(ENEMY_BB_WIDTH, ENEMY_BB_WIDTH * 4);
			float direction = (rand() % 360) * (M_PI / 180);
			vec2 tp = dirdist_extrapolate(player_motion.position, direction, distance);
			Motion test = {};
			test.position = tp;
			test.scale = motion_struct.scale;
			for (Entity e : registry.solid.entities) {
				if (collides_AABB(test, registry.motions.get(e))) {
					valid_tp = false;
					break;
				}
			}
			attempts--;
			if (valid_tp) {
				valid = true;
				motion_struct.position = tp;
				break;
			}
		}

		// fire projectile
		dir = atan2(player_motion.position.y - motion_struct.position.y, player_motion.position.x - motion_struct.position.x);
		Entity projectile = createProjectile(world.renderer, enemy, motion_struct.position, { 32, 32 }, dir, 50, TEXTURE_ASSET_ID::ORB);
		registry.colors.insert(projectile, {0.f, 0.f, 1.f, 0.7f});
		registry.solid.remove(enemy);
		registry.motions.get(enemy).in_motion = true;

		for (Entity e : registry.enemies.entities) {
			StatusEffect buff = StatusEffect(2000, 1, StatusType::RANGE_BUFF, false, true);
			apply_status(e, buff);
		}

		registry.enemies.get(enemy).state == ENEMY_STATE::ATTACK;
		boss.num_turns++;
		return;
	}

	// perform action (trust me, I'm not YandereDev, this is just a sequential state machine)
	switch (state) {
	case ENEMY_STATE::IDLE:
		printf("Turn Number %i: Doing Nothing!\n", boss.num_turns);
		if (boss.num_turns < 2) { world.logText("???: So you've made it this far...", { 1.0, 0.2, 0.2 }); }
		else if (boss.num_turns < 3) { world.logText("???: Perhaps, you may have found the answer you're looking for.", { 1.0, 0.2, 0.2 }); }
		else if (boss.num_turns < 4) { world.logText("???: It would be a shame to have gotten to this point only to fall...", { 1.0, 0.2, 0.2 }); }
		else if (boss.num_turns < 5) { 
			world.logText("???: Oh, where are my manners?", { 1.0, 0.2, 0.2 });
			world.logText("???: Let me take a form you're more familiar with...", { 1.0, 0.2, 0.2 });
		}
		else if (boss.num_turns < 6) {
			world.logText("???: So tell me, just what is it you desire?", { 1.0, 0.2, 0.2 });

			motion_struct.position = motion_struct.destination;

			// remove solid lol
			if (!registry.wobbleTimers.has(enemy)) {
				WobbleTimer& wobble = registry.wobbleTimers.emplace(enemy);
				wobble.orig_scale = motion_struct.scale;
				wobble.counter_ms = 500;
				registry.solid.remove(enemy);
			}

			// KB player
			if (player_in_range(motion_struct.position, 300.f)) {
				if (!registry.knockbacks.has(player)) {
					KnockBack& knockback = registry.knockbacks.emplace(player);
					knockback.remaining_distance = max(50.f, 350 - dist_to(motion_struct.position, player_motion.position));
					knockback.angle = atan2(player_motion.position.y - motion_struct.position.y, player_motion.position.x - motion_struct.position.x);
				}
			}

			// add hp bar 
			BossHPBar& hpbar = registry.bossHPBars.emplace(enemy);
			vec2 anchorPos = { window_width_px * 0.5f, window_height_px * (1.f / 16.f) };
			hpbar.icon = createBossIcon(world.renderer, anchorPos, TEXTURE_ASSET_ID::REFLEXION, enemy);
			hpbar.iconBacking = createBossIconBacking(world.renderer, anchorPos, enemy);
			hpbar.hpBacking = createBossHPBacking(anchorPos + vec2(0, 48), enemy);
			hpbar.hpFill = createBossHPFill(anchorPos + vec2(0, 48), enemy);

			Entity curse = createBigSlash(world.renderer, motion_struct.position, 0, 10000);
			registry.renderRequests.get(curse).used_texture = TEXTURE_ASSET_ID::CURSE;
			registry.expandTimers.get(curse).counter_ms = 500;
			registry.colors.insert(curse, {0.f, 0.f, 0.f, 1.f});

			world.playMusic(Music::BOSS1);
			registry.enemies.get(enemy).state = ENEMY_STATE::AGGRO;
		}
		break;
	case ENEMY_STATE::ATTACK:
		printf("Turn Number %i: Taking Break!\n", boss.num_turns);
		state = ENEMY_STATE::AGGRO;
		break;
	case ENEMY_STATE::AGGRO:

		// Random Attack
		roll = irand(3);
		// Choose Your Fate
		if (roll < 1) { 
			printf("Turn Number %i: Choose Your Fate!\n", boss.num_turns);
			Mix_PlayChannel(-1, world.whoosh, 0);
			// Random Dialogue
			roll = irand(3);
			if (roll < 1) { world.logText("???: Come. Take your pick.", { 1.0, 0.2, 0.2 }); }
			else if (roll < 2) { world.logText("???: Let's see what you can do.", { 1.0, 0.2, 0.2 }); }
			else { world.logText("???: Now now, you won't get forever to decide.", { 1.0, 0.2, 0.2 }); }

			for (int i = 0; i < 4; ++i) {
				float dir = i * (M_PI / 2);
				roll = irand(4);
				vec2 pos = dirdist_extrapolate(player_motion.position, dir, 120);
				Entity trap = createTrap(world.renderer, enemy, pos, { 150, 150 }, stats.atk, 2, 1, TEXTURE_ASSET_ID::FATE);

				if (roll < 1) { registry.colors.insert(trap, { 1.f, 0.f, 0.f, 0.9f }); }
				else if (roll < 2) { registry.colors.insert(trap, { 0.f, 1.f, 0.f, 0.9f }); }
				else if (roll < 3) { registry.colors.insert(trap, { 0.f, 0.f, 1.f, 0.9f }); }
				else { registry.colors.insert(trap, { 1.f, 0.f, 1.f, 0.9f }); }
			}

			createAttackIndicator(world.renderer, player_motion.position, 400, 400, true);

			registry.enemies.get(enemy).state = ENEMY_STATE::CHARGING_MELEE;
		}
		// Be Not Alone
		else if (roll < 2) { 
			printf("Turn Number %i: Be Not Alone!\n", boss.num_turns);
			Mix_PlayChannel(-1, world.malediction_sound, 0);
			// Random Dialogue
			roll = irand(3);
			if (roll < 1) { world.logText("???: Let me introduce you to my friends...", { 1.0, 0.2, 0.2 }); }
			else if (roll < 2) { world.logText("???: These ones shall keep you in good company...", { 1.0, 0.2, 0.2 }); }
			else { world.logText("???: Let's see if you can handle this.", { 1.0, 0.2, 0.2 }); }

			int num_summons = 2;
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
					// summon random monster with bias towards apparition
					roll = irand(6);
					Entity summon;
					if (roll < 1) { 
						summon = createEnemy(world.renderer, spawnpoint, true);
					}
					else if (roll < 2) { 
						summon = createPlantShooter(world.renderer, spawnpoint, true);
					}
					else if (roll < 3) {
						summon = createCaveling(world.renderer, spawnpoint, true);
					}
					else if (roll < 4) { 
						summon = createLivingRock(world.renderer, spawnpoint);
					}
					else { 
						summon = createApparition(world.renderer, spawnpoint);
					}

					reset_stats(summon);
					calc_stats(summon);
					world.turnOrderSystem.turnQueue.addNewEntity(summon);
					ExpandTimer iframe = registry.iFrameTimers.emplace(summon);
					iframe.counter_ms = 50;
					num_summons--;
				}
			}

			registry.enemies.get(enemy).state = ENEMY_STATE::CHARGING_RANGED;
		}
		// Be Not Afraid
		else { 
			printf("Turn Number %i: Be Not Afraid!\n", boss.num_turns);
			Mix_PlayChannel(-1, world.bag_of_wind_sound, 0);
			// Random Dialogue
			roll = irand(3);
			if (roll < 1) { world.logText("???: Your fear will be your downfall!", { 1.0, 0.2, 0.2 }); }
			else if (roll < 2) { world.logText("???: There's no escape!", { 1.0, 0.2, 0.2 }); }
			else { world.logText("???: Feeling afraid?", { 1.0, 0.2, 0.2 }); }

			int num_summons = 2;
			while (num_summons > 0) {
				bool valid_summon = true;
				int distance = irandRange(ENEMY_BB_WIDTH * 2, ENEMY_BB_WIDTH * 4);
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
					Entity summon = createLivingPebble(world.renderer, spawnpoint);
					Stats& summon_stats = registry.basestats.get(summon);
					summon_stats.name = "Orb of Fear";
					summon_stats.maxhp = 2400;
					summon_stats.speed = 1;
					summon_stats.atk = stats.atk;
					summon_stats.def = 999;
					summon_stats.range = 3000;
					registry.stats.get(summon).hp = summon_stats.maxhp;
					reset_stats(summon);
					calc_stats(summon);
					world.turnOrderSystem.turnQueue.addNewEntity(summon);
					registry.renderRequests.get(summon).used_texture = TEXTURE_ASSET_ID::ORB;

					ExpandTimer iframe = registry.iFrameTimers.emplace(summon);
					iframe.counter_ms = 50;
					num_summons--;
				}
			}

			registry.enemies.get(enemy).state = ENEMY_STATE::ATTACK;
		}

		break;
	case ENEMY_STATE::CHARGING_MELEE:
		printf("Turn Number %i: Triggering AoE!\n", boss.num_turns);
		for (int i = (int)registry.attackIndicators.components.size() - 1; i >= 0; --i) {
			if (player_in_range(registry.motions.get(registry.attackIndicators.entities[i]).position, registry.motions.get(registry.attackIndicators.entities[i]).scale.x / 2)) {
				world.logText(deal_damage(enemy, player, 200));
			}
			Entity explosion = createExplosion(world.renderer, registry.motions.get(registry.attackIndicators.entities[i]).position);
			registry.motions.get(explosion).scale = { 400, 400 };
			registry.colors.insert(explosion, { 0.2f, 0.2f, 0.2f, 0.5f });
			registry.remove_all_components_of(registry.attackIndicators.entities[i]);
			Mix_PlayChannel(-1, world.fire_explosion_sound, 0);
		}
		for (Entity t : registry.traps.entities) {
			if (registry.traps.get(t).owner != player) {
				registry.traps.get(t).turns -= 999;
			}
		}
		registry.enemies.get(enemy).state = ENEMY_STATE::ATTACK;
		break;
	case ENEMY_STATE::CHARGING_RANGED:
		printf("Turn Number %i: Triggering Global ATK Buff!\n", boss.num_turns);
		Mix_PlayChannel(-1, world.special_sound, 0);
		for (Entity e : registry.enemies.entities) {
			StatusEffect buff = StatusEffect(0.5, 3, StatusType::ATK_BUFF, true, true);
			apply_status(e, buff);
		}
		registry.enemies.get(enemy).state = ENEMY_STATE::ATTACK;
		break;
	case ENEMY_STATE::DEATH:
		// death
		world.logText("???: Looks like...you've found what you're looking for...", { 1.0, 0.2, 0.2 });
		world.playMusic(Music::RUINS);
		for (int i = (int)registry.attackIndicators.components.size() - 1; i >= 0; --i) {
			printf("Removed Attack Indicator!\n");
			registry.remove_all_components_of(registry.attackIndicators.entities[i]);
		}
		for (Entity t : registry.traps.entities) {
			if (registry.traps.get(t).owner != player) {
				if (registry.motions.has(t)) {
					registry.remove_all_components_of(t);
				}
			}
		}
		for (Entity e : registry.enemies.entities) {
			if (registry.bosses.has(e)) { continue; }
			take_damage(e, 999999);
		}
		// I hate C++
		if (true) {
			Entity curse = createBigSlash(world.renderer, motion_struct.position, 0, 10000);
			registry.renderRequests.get(curse).used_texture = TEXTURE_ASSET_ID::CURSE;
			registry.expandTimers.get(curse).counter_ms = 500;
			registry.colors.insert(curse, { 0.f, 0.f, 0.f, 1.f });
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
