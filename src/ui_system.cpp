#pragma once

#include "ui_system.hpp"

void UISystem::step(RenderSystem* renderer, TurnOrderSystem& tos) {
    updateQueue(renderer, tos);
}

void UISystem::initializeTurnUI(RenderSystem* renderer) {
    turnUI = createTurnUI(renderer, { window_width_px*(3.f/4.f), window_height_px*(1.f/8.f) });
}

void UISystem::updateQueue(RenderSystem* renderer, TurnOrderSystem& tos) {
    turnQueue = tos.getTurnOrder();

    // clear icons
    for (Entity e : icons) {
        registry.remove_all_components_of(e);
    }
    icons.clear();

    // update icons
    for (int count = 0; !turnQueue.empty() && count < 5; turnQueue.pop()) {
        Entity e = turnQueue.front();
        if (registry.hidden.has(e)) {
            icons.push_back(createIcon(renderer, { window_width_px*(3.f/4.f), window_height_px*(1.f/8.f) }));
        }
    }
}