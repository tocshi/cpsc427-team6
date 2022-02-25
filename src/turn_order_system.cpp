#include "turn_order_system.hpp"

// setup the turn order system
void TurnOrderSystem::setUpTurnOrder() {
	// get player and put to front of queue
	for (Entity player : registry.players.entities) {
		turnQueue.push(player);
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
	// check to see if the next thing is still alive(should still be in queueables)
	while (!registry.queueables.has(next)) {
		next = turnQueue.front();
		turnQueue.pop();
	}
	turnQueue.push(currentEntity);
	currentEntity = next;
	// set doing_turn of the entity to true
	registry.queueables.get(currentEntity).doing_turn = true;
	return next;
}
