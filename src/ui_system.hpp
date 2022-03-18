#pragma once

#include <vector>
#include <queue>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include "world_init.hpp"
#include "turn_order_system.hpp"

class UISystem
{
public:
	void step(RenderSystem* renderer, TurnOrderSystem& tos);
    void initializeTurnUI(RenderSystem* renderer);
    void updateQueue(RenderSystem* renderer, TurnOrderSystem& tos);
private:
    Entity& turnUI;
    std::queue<Entity> turnQueue;
    std::vector<Entity> icons;
};

