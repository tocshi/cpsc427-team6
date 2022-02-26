#include "turn_order_system.hpp"

// setup the turn order system
void TurnOrderSystem::setUpTurnOrder() {
	// get player and put to front of queue
	for (Entity player : registry.players.entities) {
		currentEntity = player;
	}

	// loop through all queuables and add them to turnQueue
	for (Entity e : registry.queueables.entities) {
		// if the entity is not the player, add to queue
		if (!registry.players.has(e)) {
			turnQueue.push(e);
		}
	}
}

// get the entity of the object whose turn it is
Entity TurnOrderSystem::getCurrentTurnEntity() {
	return currentEntity;
}

// get the next turn, put the entity that just did a turn to back of the queue
Entity TurnOrderSystem::getNextTurn() {
	
	Entity next;

	// push current Entity into queue just in case quueue is empty
	turnQueue.push(currentEntity);

	// check to see if the next thing is still alive(should still be in queueables)
	while (!registry.queueables.has(next)) {
		next = turnQueue.front();
		turnQueue.pop();
	}
	
	currentEntity = next;
	// set doing_turn of the entity to true
	registry.queueables.get(currentEntity).doing_turn = true;
	return next;
}

std::queue<Entity> TurnOrderSystem::getTurnOrder() {
	std::queue<Entity> returnQueue;

	// first put current entity in the front of the return queue
	returnQueue.push(currentEntity);

	// go through all the entities in the turn order and place them into return queue
	int size = turnQueue.size();
	for (int i = 0; i < size; i++) {
		// get entity
		Entity e = turnQueue.front();
		turnQueue.pop();
		// put in return queue
		returnQueue.push(e);
		// put entity to back of turn Queue (at the end it should be the same order)
		turnQueue.push(e);
	}

	return returnQueue;
}

void TurnOrderSystem::loadTurnOrder(std::queue<Entity> queue) {
	turnQueue = queue;
	currentEntity = turnQueue.front();
	turnQueue.pop();
}
