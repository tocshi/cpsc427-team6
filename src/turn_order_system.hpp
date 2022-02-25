#include "common.hpp"
#include "tiny_ecs.hpp"
#include "components.hpp"
#include "tiny_ecs_registry.hpp"
#include <iostream>
#include <queue>

class TurnOrderSystem {
public:
	void setUpTurnOrder();
	Entity getCurrentTurnEntity();
	Entity getNextTurn();

private:
	Entity currentEntity;
	std::queue<Entity> turnQueue;
};