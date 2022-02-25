#include "common.hpp"
#include "tiny_ecs.hpp"
#include "components.hpp"
#include "tiny_ecs_registry.hpp"
#include <iostream>
#include <queue>
using namespace std;

class TurnOrderSystem {
public:
	void setUpTurnOrder();
	Entity getCurrentTurnEntity();
	Entity getNextTurn();

private:
	Entity currentEntity;
	queue<Entity> turnQueue;
};