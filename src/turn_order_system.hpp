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
	std::queue<Entity> getTurnOrder();
	void loadTurnOrder(std::queue<Entity> queue);

private:
	Entity currentEntity;
	std::queue<Entity> turnQueue;
};