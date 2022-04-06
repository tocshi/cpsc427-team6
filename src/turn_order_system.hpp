#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "components.hpp"
#include "tiny_ecs_registry.hpp"
#include "turn_queue.hpp"
#include <iostream>
#include <queue>

class TurnOrderSystem {
public:
	void setUpTurnOrder();
	Entity getCurrentTurnEntity();
	Entity getNextTurn();
	std::queue<Entity> getTurnOrder();
	void loadTurnOrder(std::queue<Entity> queue);
	void removeFromQueue(Entity e);
	void emptyQueue();
	void addNewEntity(Entity e);
	TurnQueue turnQueue;
private:
	Entity currentEntity;
	bool firstTurn = false;
};