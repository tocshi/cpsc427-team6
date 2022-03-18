#include "turn_order_system.hpp"
#include "combat_system.hpp"

// setup the turn order system
void TurnOrderSystem::setUpTurnOrder() {
	firstTurn = true;
	turnQueue.setUpQueue(registry.queueables.entities);
}

// get the entity of the object whose turn it is
Entity TurnOrderSystem::getCurrentTurnEntity() {
	return currentEntity;
}

// get the next turn, put the entity that just did a turn to back of the queue
Entity TurnOrderSystem::getNextTurn() {
	
	Entity next;

	// push current Entity into queue just in case queue is empty
	// handle end-of-turn behaviour
	if (!firstTurn) {
		turnQueue.putBackEntity(currentEntity);
		handle_status_ticks(currentEntity, false);
	}

	// if this is the first turn, set first turn to false
	if (firstTurn) {
		firstTurn = false;
	}

	// check to see if the next thing is still alive(should still be in queueables)
	while (!registry.queueables.has(next)) {
		next = turnQueue.getNext();
	}

	// if the next turn is player, we need to restructure the queue
	if (registry.players.has(next)) {
		turnQueue.restructure();
	}
	
	currentEntity = next;
	// set doing_turn of the entity to true
	// handle start-of-turn behaviour
	registry.queueables.get(currentEntity).doing_turn = true;
	handle_status_ticks(currentEntity, true);
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
		Entity e = turnQueue.getNext();
		// print turn order
		/*
		if (registry.players.has(e)) {
			printf("player here\n");
		}
		if (registry.enemies.has(e)) {
			printf("enemy here\n");
		}*/
		// put in return queue
		returnQueue.push(e);
		// put entity to back of turn Queue (at the end it should be the same order)
		turnQueue.putBackEntity(e);
	}

	return returnQueue;
}

void TurnOrderSystem::loadTurnOrder(std::queue<Entity> queue) {
	while (!queue.empty()) {
		turnQueue.putBackEntity(queue.front());
		queue.pop();
	}
	currentEntity = turnQueue.getNext();
}

void TurnOrderSystem::removeFromQueue(Entity e) {
	turnQueue.removeEntity(e);
}

void TurnOrderSystem::emptyQueue() {
	turnQueue.emptyQueue();
}