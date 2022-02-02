#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are ahrd coded to the dimensions of the entity texture
const float BUG_BB_WIDTH = 0.6f * 165.f;
const float BUG_BB_HEIGHT = 0.6f * 165.f;
const float EAGLE_BB_WIDTH = 0.6f * 300.f;
const float EAGLE_BB_HEIGHT = 0.6f * 202.f;

// Some are placeholder
const float PLAYER_BB_WIDTH = 0.6f * 150.f;
const float PLAYER_BB_HEIGHT = 0.6f * 150.f;
const float ENEMY_BB_WIDTH = 0.6f * 150.f;
const float ENEMY_BB_HEIGHT = 0.6f * 150.f;
const float BOSS_BB_WIDTH = 0.6f * 150.f;
const float BOSS_BB_HEIGHT = 0.6f * 150.f;
const float ARTIFACT_BB_WIDTH = 0.6f * 150.f;
const float ARTIFACT_BB_HEIGHT = 0.6f * 150.f;
const float CONSUMABLE_BB_WIDTH = 0.6f * 150.f;
const float CONSUMABLE_BB_HEIGHT = 0.6f * 150.f;
const float EQUIPABLE_BB_WIDTH = 0.6f * 150.f;
const float EQUIPABLE_BB_HEIGHT = 0.6f * 150.f;
const float CHEST_BB_WIDTH = 0.6f * 150.f;
const float CHEST_BB_HEIGHT = 0.6f * 150.f;
const float DOOR_BB_WIDTH = 0.6f * 150.f;
const float DOOR_BB_HEIGHT = 0.6f * 150.f;
const float SIGN_BB_WIDTH = 0.6f * 150.f;
const float SIGN_BB_HEIGHT = 0.6f * 150.f;
const float STAIR_BB_WIDTH = 0.6f * 150.f;
const float STAIR_BB_HEIGHT = 0.6f * 150.f;
const float START_BB_WIDTH = 0.6f * 900.f;
const float START_BB_HEIGHT = 0.6f * 150.f;

// the player
Entity createChicken(RenderSystem* renderer, vec2 pos);
// the prey
Entity createBug(RenderSystem* renderer, vec2 position);
// the enemy
Entity createEagle(RenderSystem* renderer, vec2 position);
// a red line for debugging purposes
Entity createLine(vec2 position, vec2 size);
// a egg
Entity createEgg(vec2 pos, vec2 size);
// the background
Entity createBackground(RenderSystem* renderer, vec2 position);

// ================================
// Player
Entity createPlayer(RenderSystem* renderer, vec2 pos);
// Enemy (split into different enemies for future)
Entity createEnemy(RenderSystem* renderer, vec2 pos);
// Boss
Entity createBoss(RenderSystem* renderer, vec2 pos);
// Artifact
Entity createArtifact(RenderSystem* renderer, vec2 pos);
// Item (consumable)
Entity createConsumable(RenderSystem* renderer, vec2 pos);
// Item (equipable)
Entity createEquipable(RenderSystem* renderer, vec2 pos);
// Chest
Entity createChest(RenderSystem* renderer, vec2 pos);
// Door
Entity createDoor(RenderSystem* renderer, vec2 pos);
// Sign
Entity createSign(RenderSystem* renderer, vec2 pos);
// Stair
Entity createStair(RenderSystem* renderer, vec2 pos);
// Menu start
Entity createMenuStart(RenderSystem* renderer, vec2 pos);
